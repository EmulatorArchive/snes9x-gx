/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * crunchy2 April 2007-July 2007
 * Michniewski 2008
 * Tantric September 2008
 *
 * sram.cpp
 *
 * SRAM save/load/import/export handling
 ***************************************************************************/

bool SaveSRAM (char * filepath, bool silent);
bool SaveSRAMAuto (bool silent);
bool LoadSRAM (char * filepath, bool silent);
bool LoadSRAMAuto (bool silent);
