MAPI Property Tag List
======================

Definitions
-----------

* property _tag_: 32-bit unsigned integer, composed of:
  * bits 31..16: 16-bit property _ID_
  * bits 15..0: 16-bit property _type_
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
