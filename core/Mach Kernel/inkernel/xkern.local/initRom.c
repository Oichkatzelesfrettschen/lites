
void
initRom()
{
	if ( ROM_MAX_LINES < 9 ) {
		xError("xkernel ROM array too big");
		return;
	}
	if ( ROM_MAX_FIELDS < 3 ) {
		xError("xkernel ROM -- too many fields");
		return;
	}
	rom[0][0] = "arp";
	rom[0][1] = "192.12.69.54";
	rom[0][2] = "08:00:2b:1c:19:cd";
	rom[0][3] = "lock";
	rom[0][4] = 0;
	rom[1][0] = "arp";
	rom[1][1] = "192.12.69.137";
	rom[1][2] = "08:00:2b:1b:c4:bb";
	rom[1][3] = "lock";
	rom[1][4] = 0;
	rom[2][0] = "arp";
	rom[2][1] = "192.12.69.98";
	rom[2][2] = "08:00:2b:1c:18:e8";
	rom[2][3] = "lock";
	rom[2][4] = 0;
	rom[3][0] = "arp";
	rom[3][1] = "192.12.69.99";
	rom[3][2] = "08:00:2b:23:6d:ec";
	rom[3][3] = "lock";
	rom[3][4] = 0;
	rom[4][0] = "arp";
	rom[4][1] = "192.12.69.10";
	rom[4][2] = "08:00:09:27:b4:2d";
	rom[4][3] = "lock";
	rom[4][4] = 0;
	rom[5][0] = "arp";
	rom[5][1] = "192.12.69.24";
	rom[5][2] = "08:00:09:27:b4:a6";
	rom[5][3] = "lock";
	rom[5][4] = 0;
	rom[6][0] = "arp";
	rom[6][1] = "192.12.69.26";
	rom[6][2] = "08:00:09:27:c4:49";
	rom[6][3] = "lock";
	rom[6][4] = 0;
	rom[7][0] = "arp";
	rom[7][1] = "192.12.69.25";
	rom[7][2] = "08:00:09:27:f4:7f";
	rom[7][3] = "lock";
	rom[7][4] = 0;
	rom[8][0] = "arp";
	rom[8][1] = "192.12.69.14";
	rom[8][2] = "08:00:09:27:c4:47";
	rom[8][3] = "lock";
	rom[8][4] = 0;
	rom[9][0] = 0;
}
