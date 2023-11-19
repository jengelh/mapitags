MAPI Property Tag List
======================

mapitags.txt is a list of MAPI property tags that Microsoft products exercise
or reference. For some of the tags, semantics are documented in MS-OXPROPS (or
related documents; MS-OXPROTLP). For some of the tags, only the mnemonic name
is known because some program referenced the number. And for some of the tags,
only the number is known, but where the semantics have been decoded without
outside assistance.

namedproptags.txt is a (likely very incomplete) list for Named Properties'
LID/Names and types.

Property tags which are specific to *vendors other than Microsoft* are in
separate .txt files.

Mnemonic names in the text file can have different forms:

* ``PR_xxx`` are names exposed by the SDKs for the C programming language. The
  "Inside MAPI" book (1996) also uses the C name.
* ``PidTagXXX`` is used in documentation and referred to as the "canonical name".
* ``ptagXXX`` shows up in documentation, the reason for this duplication is
  unclear. I suspect style wars in Microsoft source codes.
* ``.XXX`` are names exposed by C# programming language interfaces.
* When a property tag has a PR-style name or a PidTag-style canonical name
  already, I have omitted listing the ptag-style variation.


Definitions
-----------

* property *tag*: 32-bit unsigned integer, composed of:

  * bits 31..16: 16-bit property *ID*

  * bits 15..0: 16-bit property *type*

    * bit 13: instance flag

    * bit 12: multivalue flag

    * bits 11..0: (no specific name)

MAPI property types and OLE types [MS-OAUT] share some values.


General grouping
----------------

.. code-block::

	Reserved:                          0x0000
	Tagged props:                      0x0001..0x7FFF
	 ┣━ MAPI-defined props:            0x0001..0x3FFF
	 ┃   ┣━ Envelope props:            0x0001..0x0BFF
	 ┃   ┣━ Recipient props:           0x0C00..0x0DFF
	 ┃   ┣━ Non-transmittable props:   0x0E00..0x0FFF (non-transmittable)
	 ┃   ┣━ Message content props:     0x1000..0x2FFF
	 ┃   ┗━ Others:                    0x3000..0x3FFF
	 ┃       ┣━ Common props:          0x3000..0x32FF
	 ┃       ┣━ Form props:            0x3300..0x33FF
         ┃       ┣━ Message store:         0x3400..0x35FF
         ┃       ┣━ Container (folder/AB): 0x3600..0x36FF
         ┃       ┣━ Attachment:            0x3700..0x38FF
         ┃       ┣━ Address book:          0x3900..0x39FF
         ┃       ┣━ Mail user:             0x3A00..0x3BFF
         ┃       ┣━ Distribution list:     0x3C00..0x3CFF
         ┃       ┣━ Profsect:              0x3D00..0x3DFF
         ┃       ┣━ Status object:         0x3E00..0x3EFF
         ┃       ┗━ Display table:         0x3F00..0x3FFF
         ┣━ Transport-specific props:      0x4000..0x5FFF
         ┃   ┣━ Envelope props:            0x4000..0x57FF
         ┃   ┗━ Recipient props:           0x5800..0x5FFF
         ┣━ Client-specific props:         0x6000..0x65FF (non-transmittable)
         ┣━ Provider-specific props:       0x6600..0x67FF (non-transmittable)
         ┃   ┗━ Secure profile props:      0x67F0..0x67FF
         ┗━ Message class-specific props:  0x6800..0x7FFF
             ┣━ Content props:             0x6800..0x7BFF
             ┗━ Non-transmittable:         0x7C00..0x7FFF (non-transmittable)
        Mapping range for named props:     0x8000..0xFFFE
        Reserved:                          0xFFFF

There are some more reserved ranges, but we need not bother with it.
NSPI does not have named properties, so 0x8000..0xFFFE are just more
properties there.
