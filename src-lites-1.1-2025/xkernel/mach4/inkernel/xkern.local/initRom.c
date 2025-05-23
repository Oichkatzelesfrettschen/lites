
void
initRom()
{
	if ( ROM_MAX_LINES < 21 ) {
		xError("xkernel ROM array too big");
		return;
	}
	if ( ROM_MAX_FIELDS < 3 ) {
		xError("xkernel ROM -- too many fields");
		return;
	}
	rom[0][0] = "ethdrv/wd0";
	rom[0][1] = "priority";
	rom[0][2] = "20";
	rom[0][3] = 0;
	rom[1][0] = "arp";
	rom[1][1] = "193.209.237.25";
	rom[1][2] = "0:0:c0:9:dd:15";
	rom[1][3] = "lock";
	rom[1][4] = 0;
	rom[2][0] = "arp";
	rom[2][1] = "193.209.237.1";
	rom[2][2] = "0:0:c:2:7:70";
	rom[2][3] = 0;
	rom[3][0] = "ip";
	rom[3][1] = "gateway";
	rom[3][2] = "193.209.237.1";
	rom[3][3] = 0;
	rom[4][0] = "netmask";
	rom[4][1] = "193.209.237.25";
	rom[4][2] = "255.255.255.0";
	rom[4][3] = 0;
	rom[5][0] = "dns";
	rom[5][1] = "koff";
	rom[5][2] = "193.209.237.25";
	rom[5][3] = 0;
	rom[6][0] = "dns";
	rom[6][1] = "olvi";
	rom[6][2] = "193.209.237.26";
	rom[6][3] = 0;
	rom[7][0] = "dns";
	rom[7][1] = "jalopeno";
	rom[7][2] = "193.209.237.20";
	rom[7][3] = 0;
	rom[8][0] = "dns";
	rom[8][1] = "tequila";
	rom[8][2] = "193.209.237.23";
	rom[8][3] = 0;
	rom[9][0] = "dns";
	rom[9][1] = "koff.nixu.fi";
	rom[9][2] = "193.209.237.25";
	rom[9][3] = 0;
	rom[10][0] = "dns";
	rom[10][1] = "olvi.nixu.fi";
	rom[10][2] = "193.209.237.26";
	rom[10][3] = 0;
	rom[11][0] = "dns";
	rom[11][1] = "jalopeno.nixu.fi";
	rom[11][2] = "193.209.237.20";
	rom[11][3] = 0;
	rom[12][0] = "dns";
	rom[12][1] = "tequila.nixu.fi";
	rom[12][2] = "193.209.237.23";
	rom[12][3] = 0;
	rom[13][0] = "dns";
	rom[13][1] = "juustolettu";
	rom[13][2] = "130.233.192.249";
	rom[13][3] = 0;
	rom[14][0] = "dns";
	rom[14][1] = "papuherkku";
	rom[14][2] = "130.233.192.250";
	rom[14][3] = 0;
	rom[15][0] = "dns";
	rom[15][1] = "tomaattikeitto";
	rom[15][2] = "130.233.192.251";
	rom[15][3] = 0;
	rom[16][0] = "dns";
	rom[16][1] = "cs-gw";
	rom[16][2] = "130.233.192.254";
	rom[16][3] = 0;
	rom[17][0] = "dns";
	rom[17][1] = "juustolettu.cs.hut.fi";
	rom[17][2] = "130.233.192.249";
	rom[17][3] = 0;
	rom[18][0] = "dns";
	rom[18][1] = "papuherkku.cs.hut.fi";
	rom[18][2] = "130.233.192.250";
	rom[18][3] = 0;
	rom[19][0] = "dns";
	rom[19][1] = "tomaattikeitto.cs.hut.fi";
	rom[19][2] = "130.233.192.251";
	rom[19][3] = 0;
	rom[20][0] = "dns";
	rom[20][1] = "cs-gw.cs.hut.fi";
	rom[20][2] = "130.233.192.254";
	rom[20][3] = 0;
	rom[21][0] = 0;
}
