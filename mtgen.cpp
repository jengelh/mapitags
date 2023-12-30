// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Jan Engelhardt
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

enum { M_ENUM, M_CONSTEXPR, M_DEFINE, };
enum { FL_A = 0x01, FL_W = 0x02, FL_O = 0x04, FL_MV = 0x08 };

static bool g_pretty, g_allnames;
static unsigned int g_mode;

static uint32_t parse_flags(const char *s, char **end)
{
	uint32_t fl = 0;
	for (; *s != '\0' && !isspace(*s); ++s) {
		if (*s == 'A') {
			fl |= FL_A;
		} else if (*s == 'W') {
			fl |= FL_W;
		} else if (*s == 'O') {
			fl |= FL_O;
		} else if (s[0] == 'M' && s[1] == 'V') {
			fl |= FL_MV;
			++s;
		}
	}
	*end = const_cast<char *>(s);
	return fl;
}

static const char *pt_lookup(uint16_t type)
{
	switch (type) {
	case 0x0: return "PT_UNSPECIFIED"; /* VT_EMPTY, PtypUnspecified */
	case 0x1: return "PT_NULL"; /* VT_NULL, PtypNull */
	case 0x2: return "PT_SHORT"; /* VT_I2, PT_I2, PtypInteger16 */
	case 0x3: return "PT_LONG"; /* VT_I4, PT_I4, PtypInteger32 */
	case 0x4: return "PT_FLOAT"; /* VT_R4, PT_R4, PtypFloating32 */
	case 0x5: return "PT_DOUBLE"; /* VT_R8, PT_R8, PtypFloating64 */
	case 0x6: return "PT_CURRENCY"; /* VT_CY, PtypCurrency */
	case 0x7: return "PT_APPTIME"; /* VT_DATE, PtypFloatingTime */
	case 0x8: return "VT_BSTR";
	case 0x9: return "VT_DISPATCH";
	case 0xa: return "PT_ERROR"; /* VT_ERROR, PtypErrorCode */
	case 0xb: return "PT_BOOLEAN";
	case 0xc: return "VT_VARIANT";
	case 0xd: return "PT_OBJECT"; /* VT_UNKNOWN, PtypObject, PtypEmbeddedTable */
	case 0xe: return "VT_DECIMAL";
	case 0x10: return "VT_I1";
	case 0x11: return "VT_UI1";
	case 0x12: return "VT_UI2";
	case 0x13: return "VT_UI4";
	case 0x14: return "PT_I8"; /* VT_I8, PtypInteger64 */
	case 0x15: return "VT_UI8";
	case 0x16: return "VT_INT";
	case 0x17: return "VT_UINT";
	case 0x18: return "VT_VOID";
	case 0x19: return "VT_HRESULT";
	case 0x1a: return "VT_PTR";
	case 0x1b: return "VT_SAFEARRAY";
	case 0x1c: return "VT_CARRAY";
	case 0x1d: return "VT_USERDEFINED";
	case 0x1e: return "PT_STRING8"; /* VT_LPSTR, PtypString8 */
	case 0x1f: return "PT_UNICODE"; /* VT_LPWSTR, PtypString */
	case 0x24: return "VT_RECORD";
	case 0x25: return "VT_INT_PTR";
	case 0x26: return "VT_UINT_PTR";
	case 0x40: return "PT_SYSTIME"; /* VT_FILETIME, PtypTime */
	case 0x41: return "VT_BLOB";
	case 0x42: return "VT_STREAM";
	case 0x43: return "VT_STORAGE";
	case 0x44: return "VT_STREAMED_OBJECT";
	case 0x45: return "VT_STORED_OBJECT";
	case 0x46: return "VT_BLOB_OBJECT";
	case 0x47: return "VT_CF";
	case 0x48: return "PT_CLSID"; /* VT_CLSID, PtypGuid */
	case 0x49: return "VT_VERSIONED_STREAM";
	case 0xfa: return "RuntimeObject";
	case 0xfb: return "PT_SVREID"; /* PtypServerId; MS-OXCDATA extension */
	case 0xfd: return "PT_SRESTRICTION"; /* PtypRestriction; edkmdb extension */
	case 0xfe: return "PT_ACTIONS"; /* PtypRuleAction; edkmdb extension */
	case 0x102: return "PT_BINARY"; /* PtypBinary */
	case 0x103: return "PT_FILE_HANDLE"; /* edkmdb.h extension; (SPropValue::Value.lpv has the file handle) */
	case 0x104: return "PT_FILE_EA"; /* edkmdb.h extension; (SPropValue::Value.bin has extended attribute data for locating the file) */
	case 0x105: return "PT_VIRTUAL"; /* edkmdb.h extension; (SPropValue::Value.bin has arbitrary data; store-internal; not externally visible) */
	case 0xfff: return "VT_BLOB_BSTR";
	default: fprintf(stderr, "Unrecognized type 0x%x\n", type); abort();
	}
}

static void emit_def(const char *name, uint32_t proptag)
{
	switch (g_mode) {
	case M_ENUM: printf("\t%s = ", name); break;
	case M_CONSTEXPR: printf("constexpr uint32_t %s = ", name); break;
	case M_DEFINE: printf("#define %s ", name); break;
	}
	if (!g_pretty) {
		printf("0x%08x", proptag);
	} else {
		auto pt_name = pt_lookup(proptag & 0xfff);
		if (pt_name == nullptr) {
			printf("PROP_TAG(0x%x, ", proptag & 0xffff);
		} else {
			auto mv = proptag & 0x1000 ? "MV_" : ""; /* VT_VECTOR */
			auto i  = proptag & 0x2000 ? " | MV_INSTANCE" : ""; /* VT_ARRAY */
			printf("PROP_TAG(%.3s%s%s%s, ", pt_name, mv, pt_name + 3, i);
		}
		printf("0x%04x)", (proptag >> 16) & 0xFFFF);
	}
	switch (g_mode) {
	case M_ENUM: printf(",\n"); break;
	case M_CONSTEXPR: printf(";\n"); break;
	case M_DEFINE: printf("\n"); break;
	}
}

static void parse_line(const char *input)
{
	char *p = nullptr;
	uint32_t proptag = strtoul(input, &p, 16);
	uint32_t flags = 0;
	if (*p == '+')
		flags = parse_flags(p + 1, &p);
	while (isspace(*p))
		++p;

	std::vector<std::string> name_list;
	while (*p != '\0') {
		auto start = p;
		while (*p != '\0' && !isspace(*p))
			++p;
		if (p - start == 0 || (start[0] == '/' && start[1] == '*'))
			break;
		if (*start == '.') {
			++start;
			name_list.emplace_back("PidTag" + std::string(start, p - start));
		} else {
			name_list.emplace_back(start, p - start);
		}
		while (isspace(*p))
			++p;
	}

	for (const auto &name : name_list) {
		auto np = name.c_str();
		if (np[0] == '?')
			continue;
		emit_def(np, proptag);
		if (np[0] != 'P' && np[1] != 'R' && np[2] != '_')
			continue;
		if (flags & FL_A)
			emit_def((name + "_A").c_str(), (proptag & 0xffff0000) | 0x1e);
		if (flags & FL_W)
			emit_def((name + "_W").c_str(), (proptag & 0xffff0000) | 0x1f);
		if (flags & FL_O)
			emit_def((name + "_O").c_str(), (proptag & 0xffff0000) | 0x0d);
		if (flags & FL_MV)
			emit_def((name + "_MV").c_str(), (proptag & 0xffff0000) | 0x101f);
		if (!g_allnames)
			break;
	}
}

int main(int argc, char **argv)
{
	g_mode = M_ENUM;
	int c;
	while ((c = getopt(argc, argv, "acdep")) >= 0) {
		if (c == 'a')
			g_allnames = true;
		else if (c == 'c')
			g_mode = M_CONSTEXPR;
		else if (c == 'd')
			g_mode = M_DEFINE;
		else if (c == 'e')
			g_mode = M_ENUM;
		else if (c == 'p')
			g_pretty = true;
	}
	std::string linebuf;
	if (g_mode == M_ENUM)
		printf("enum {\n");
	while (!std::cin.eof()) {
		std::getline(std::cin, linebuf);
		parse_line(linebuf.c_str());
	}
	if (g_mode == M_ENUM)
		printf("};\n");
	return EXIT_SUCCESS;
}
