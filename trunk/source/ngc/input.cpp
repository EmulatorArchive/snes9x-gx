/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May-June 2007
 * Michniewski 2008
 * Tantric September 2008
 *
 * input.cpp
 *
 * Wii/Gamecube controller management
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "snes9x.h"
#include "memmap.h"
#include "controls.h"

#include "snes9xGX.h"
#include "button_mapping.h"
#include "s9xconfig.h"
#include "menu.h"
#include "video.h"
#include "input.h"

/****************************************************************************
 * Controller Functions
 *
 * The following map the NGC Pads to the *NEW* controller system.
 ***************************************************************************/
#define ASSIGN_BUTTON_TRUE( keycode, snescmd ) \
	  S9xMapButton( keycode, cmd = S9xGetCommandT(snescmd), true)

#define ASSIGN_BUTTON_FALSE( keycode, snescmd ) \
	  S9xMapButton( keycode, cmd = S9xGetCommandT(snescmd), false)

int scopeTurbo = 0; // tracks whether superscope turbo is on or off
unsigned int gcpadmap[12]; // Gamecube controller Padmap
unsigned int wmpadmap[12]; // Wiimote Padmap
unsigned int ccpadmap[12]; // Classic Controller Padmap
unsigned int ncpadmap[12]; // Nunchuk + wiimote Padmap
unsigned int gcscopemap[6]; // Superscope : GC controller button mapping
unsigned int wmscopemap[6]; // Superscope : wiimote button mapping
unsigned int gcmousemap[2]; // Mouse : GC controller button mapping
unsigned int wmmousemap[2]; // Mouse : wiimote button mapping
unsigned int gcjustmap[3]; // Justifier : GC controller button mapping
unsigned int wmjustmap[3]; // Justifier : wiimote button mapping

void ResetControls()
{
	int i;
	/*** Gamecube controller Padmap ***/
	i=0;
	gcpadmap[i++] = PAD_BUTTON_A;
	gcpadmap[i++] = PAD_BUTTON_B;
	gcpadmap[i++] = PAD_BUTTON_X;
	gcpadmap[i++] = PAD_BUTTON_Y;
	gcpadmap[i++] = PAD_TRIGGER_L;
	gcpadmap[i++] = PAD_TRIGGER_R;
	gcpadmap[i++] = PAD_TRIGGER_Z;
	gcpadmap[i++] = PAD_BUTTON_START;
	gcpadmap[i++] = PAD_BUTTON_UP;
	gcpadmap[i++] = PAD_BUTTON_DOWN;
	gcpadmap[i++] = PAD_BUTTON_LEFT;
	gcpadmap[i++] = PAD_BUTTON_RIGHT;

	/*** Wiimote Padmap ***/
	i=0;
	wmpadmap[i++] = WPAD_BUTTON_B;
	wmpadmap[i++] = WPAD_BUTTON_2;
	wmpadmap[i++] = WPAD_BUTTON_1;
	wmpadmap[i++] = WPAD_BUTTON_A;
	wmpadmap[i++] = 0x0000;
	wmpadmap[i++] = 0x0000;
	wmpadmap[i++] = WPAD_BUTTON_MINUS;
	wmpadmap[i++] = WPAD_BUTTON_PLUS;
	wmpadmap[i++] = WPAD_BUTTON_RIGHT;
	wmpadmap[i++] = WPAD_BUTTON_LEFT;
	wmpadmap[i++] = WPAD_BUTTON_UP;
	wmpadmap[i++] = WPAD_BUTTON_DOWN;

	/*** Classic Controller Padmap ***/
	i=0;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_A;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_B;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_X;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_Y;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_FULL_L;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_FULL_R;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_MINUS;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_PLUS;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_UP;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_DOWN;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_LEFT;
	ccpadmap[i++] = WPAD_CLASSIC_BUTTON_RIGHT;

	/*** Nunchuk + wiimote Padmap ***/
	i=0;
	ncpadmap[i++] = WPAD_BUTTON_A;
	ncpadmap[i++] = WPAD_BUTTON_B;
	ncpadmap[i++] = WPAD_NUNCHUK_BUTTON_C;
	ncpadmap[i++] = WPAD_NUNCHUK_BUTTON_Z;
	ncpadmap[i++] = WPAD_BUTTON_2;
	ncpadmap[i++] = WPAD_BUTTON_1;
	ncpadmap[i++] = WPAD_BUTTON_MINUS;
	ncpadmap[i++] = WPAD_BUTTON_PLUS;
	ncpadmap[i++] = WPAD_BUTTON_UP;
	ncpadmap[i++] = WPAD_BUTTON_DOWN;
	ncpadmap[i++] = WPAD_BUTTON_LEFT;
	ncpadmap[i++] = WPAD_BUTTON_RIGHT;

	/*** Superscope : GC controller button mapping ***/
	i=0;
	gcscopemap[i++] = PAD_TRIGGER_Z;
	gcscopemap[i++] = PAD_BUTTON_B;
	gcscopemap[i++] = PAD_BUTTON_A;
	gcscopemap[i++] = PAD_BUTTON_Y;
	gcscopemap[i++] = PAD_BUTTON_X;
	gcscopemap[i++] = PAD_BUTTON_START;

	/*** Superscope : wiimote button mapping ***/
	i=0;
	wmscopemap[i++] = WPAD_BUTTON_MINUS;
	wmscopemap[i++] = WPAD_BUTTON_B;
	wmscopemap[i++] = WPAD_BUTTON_A;
	wmscopemap[i++] = WPAD_BUTTON_UP;
	wmscopemap[i++] = WPAD_BUTTON_DOWN;
	wmscopemap[i++] = WPAD_BUTTON_PLUS;

	/*** Mouse : GC controller button mapping ***/
	i=0;
	gcmousemap[i++] = PAD_BUTTON_A;
	gcmousemap[i++] = PAD_BUTTON_B;

	/*** Mouse : wiimote button mapping ***/
	i=0;
	wmmousemap[i++] = WPAD_BUTTON_A;
	wmmousemap[i++] = WPAD_BUTTON_B;

	/*** Justifier : GC controller button mapping ***/
	i=0;
	gcjustmap[i++] = PAD_BUTTON_A;
	gcjustmap[i++] = PAD_BUTTON_B;
	gcjustmap[i++] = PAD_BUTTON_START;

	/*** Justifier : wiimote button mapping ***/
	i=0;
	wmjustmap[i++] = WPAD_BUTTON_A;
	wmjustmap[i++] = WPAD_BUTTON_B;
	wmjustmap[i++] = WPAD_BUTTON_PLUS;
}

/****************************************************************************
 * WPAD_Stick
 *
 * Get X/Y value from Wii Joystick (classic, nunchuk) input
 ***************************************************************************/

s8 WPAD_Stick(u8 chan, u8 right, int axis)
{
	float mag = 0.0;
	float ang = 0.0;
	WPADData *data = WPAD_Data(chan);

	switch (data->exp.type)
	{
		case WPAD_EXP_NUNCHUK:
		case WPAD_EXP_GUITARHERO3:
			if (right == 0)
			{
				mag = data->exp.nunchuk.js.mag;
				ang = data->exp.nunchuk.js.ang;
			}
			break;

		case WPAD_EXP_CLASSIC:
			if (right == 0)
			{
				mag = data->exp.classic.ljs.mag;
				ang = data->exp.classic.ljs.ang;
			}
			else
			{
				mag = data->exp.classic.rjs.mag;
				ang = data->exp.classic.rjs.ang;
			}
			break;

		default:
			break;
	}

	/* calculate x/y value (angle need to be converted into radian) */
	if (mag > 1.0) mag = 1.0;
	else if (mag < -1.0) mag = -1.0;
	double val;

	if(axis == 0) // x-axis
		val = mag * sin((PI * ang)/180.0f);
	else // y-axis
		val = mag * cos((PI * ang)/180.0f);

	return (s8)(val * 128.0f);
}

// hold superscope/mouse/justifier cursor positions
static int cursor_x[5] = {0,0,0,0,0};
static int cursor_y[5] = {0,0,0,0,0};

/****************************************************************************
 * UpdateCursorPosition
 *
 * Updates X/Y coordinates for Superscope/mouse/justifier position
 ***************************************************************************/

void UpdateCursorPosition (int pad, int &pos_x, int &pos_y)
{
	#define SCOPEPADCAL 20

	// gc left joystick
	signed char pad_x = PAD_StickX (pad);
	signed char pad_y = PAD_StickY (pad);

	if (pad_x > SCOPEPADCAL){
		pos_x += (pad_x*1.0)/SCOPEPADCAL;
		if (pos_x > 256) pos_x = 256;
	}
	if (pad_x < -SCOPEPADCAL){
		pos_x -= (pad_x*-1.0)/SCOPEPADCAL;
		if (pos_x < 0) pos_x = 0;
	}

	if (pad_y < -SCOPEPADCAL){
		pos_y += (pad_y*-1.0)/SCOPEPADCAL;
		if (pos_y > 224) pos_y = 224;
	}
	if (pad_y > SCOPEPADCAL){
		pos_y -= (pad_y*1.0)/SCOPEPADCAL;
		if (pos_y < 0) pos_y = 0;
	}

#ifdef HW_RVL
	struct ir_t ir;		// wiimote ir
	WPAD_IR(pad, &ir);
	if (ir.valid)
	{
		pos_x = (ir.x * 256) / 640;
		pos_y = (ir.y * 224) / 480;
	}
	else
	{
		signed char wm_ax = WPAD_Stick (pad, 0, 0);
		signed char wm_ay = WPAD_Stick (pad, 0, 1);

		if (wm_ax > SCOPEPADCAL){
			pos_x += (wm_ax*1.0)/SCOPEPADCAL;
			if (pos_x > 256) pos_x = 256;
		}
		if (wm_ax < -SCOPEPADCAL){
			pos_x -= (wm_ax*-1.0)/SCOPEPADCAL;
			if (pos_x < 0) pos_x = 0;
		}

		if (wm_ay < -SCOPEPADCAL){
			pos_y += (wm_ay*-1.0)/SCOPEPADCAL;
			if (pos_y > 224) pos_y = 224;
		}
		if (wm_ay > SCOPEPADCAL){
			pos_y -= (wm_ay*1.0)/SCOPEPADCAL;
			if (pos_y < 0) pos_y = 0;
		}
	}
#endif

}

/****************************************************************************
 * decodepad
 *
 * Reads the changes (buttons pressed, etc) from a controller and reports
 * these changes to Snes9x
 ***************************************************************************/

void decodepad (int pad)
{
	int i, offset;
	float t;

	signed char pad_x = PAD_StickX (pad);
	signed char pad_y = PAD_StickY (pad);
	u32 jp = PAD_ButtonsHeld (pad);

#ifdef HW_RVL
	signed char wm_ax = 0;
	signed char wm_ay = 0;
	u32 wp = 0;
	wm_ax = WPAD_Stick ((u8)pad, 0, 0);
	wm_ay = WPAD_Stick ((u8)pad, 0, 1);
	wp = WPAD_ButtonsHeld (pad);

	u32 exp_type;
	if ( WPAD_Probe(pad, &exp_type) != 0 )
		exp_type = WPAD_EXP_NONE;
#endif

	/***
	Gamecube Joystick input
	***/
	// Is XY inside the "zone"?
	if (pad_x * pad_x + pad_y * pad_y > PADCAL * PADCAL)
	{
		/*** we don't want division by zero ***/
		if (pad_x > 0 && pad_y == 0)
			jp |= PAD_BUTTON_RIGHT;
		if (pad_x < 0 && pad_y == 0)
			jp |= PAD_BUTTON_LEFT;
		if (pad_x == 0 && pad_y > 0)
			jp |= PAD_BUTTON_UP;
		if (pad_x == 0 && pad_y < 0)
			jp |= PAD_BUTTON_DOWN;

		if (pad_x != 0 && pad_y != 0)
		{
			/*** Recalc left / right ***/
			t = (float) pad_y / pad_x;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (pad_x >= 0)
					jp |= PAD_BUTTON_RIGHT;
				else
					jp |= PAD_BUTTON_LEFT;
			}

			/*** Recalc up / down ***/
			t = (float) pad_x / pad_y;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (pad_y >= 0)
					jp |= PAD_BUTTON_UP;
				else
					jp |= PAD_BUTTON_DOWN;
			}
		}
	}
#ifdef HW_RVL
	/***
	Wii Joystick (classic, nunchuk) input
	***/
	// Is XY inside the "zone"?
	if (wm_ax * wm_ax + wm_ay * wm_ay > PADCAL * PADCAL)
	{

	    if (wm_ax > 0 && wm_ay == 0)
			wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_RIGHT : WPAD_BUTTON_RIGHT;
	    if (wm_ax < 0 && wm_ay == 0)
			wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_LEFT : WPAD_BUTTON_LEFT;
	    if (wm_ax == 0 && wm_ay > 0)
			wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_UP : WPAD_BUTTON_UP;
	    if (wm_ax == 0 && wm_ay < 0)
			wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_DOWN : WPAD_BUTTON_DOWN;

	    if (wm_ax != 0 && wm_ay != 0)
		{
			/*** Recalc left / right ***/
			t = (float) wm_ay / wm_ax;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (wm_ax >= 0)
				{
					wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_RIGHT : WPAD_BUTTON_RIGHT;
				}
				else
				{
					wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_LEFT : WPAD_BUTTON_LEFT;
				}
			}

			/*** Recalc up / down ***/
			t = (float) wm_ax / wm_ay;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (wm_ay >= 0)
				{
					wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_UP : WPAD_BUTTON_UP;
				}
				else
				{
					wp |= (exp_type == WPAD_EXP_CLASSIC) ? WPAD_CLASSIC_BUTTON_DOWN : WPAD_BUTTON_DOWN;
				}
			}
		}
	}
#endif

	/*** Fix offset to pad ***/
	offset = ((pad + 1) << 4);

	/*** Report pressed buttons (gamepads) ***/
	for (i = 0; i < MAXJP; i++)
    {
		if ( (jp & gcpadmap[i])											// gamecube controller
#ifdef HW_RVL
		|| ( (exp_type == WPAD_EXP_NONE) && (wp & wmpadmap[i]) )	// wiimote
		|| ( (exp_type == WPAD_EXP_CLASSIC) && (wp & ccpadmap[i]) )	// classic controller
		|| ( (exp_type == WPAD_EXP_NUNCHUK) && (wp & ncpadmap[i]) )	// nunchuk + wiimote
#endif
		)
			S9xReportButton (offset + i, true);
		else
			S9xReportButton (offset + i, false);
    }

	/*** Superscope ***/
	if (Settings.SuperScopeMaster && pad == GCSettings.Superscope - 1) // report only once
	{
		// buttons
		offset = 0x50;
		for (i = 0; i < 5; i++)
		{
			if (jp & gcscopemap[i]
#ifdef HW_RVL
			|| wp & wmscopemap[i]
#endif
			)
			{
				if(i == 3 || i == 4) // turbo
				{
					if((i == 3 && scopeTurbo == 1) || // turbo ON already, don't change
						(i == 4 && scopeTurbo == 0)) // turbo OFF already, don't change
					{
						S9xReportButton(offset + i, false);
					}
					else // turbo changed to ON or OFF
					{
						scopeTurbo = 4-i;
						S9xReportButton(offset + i, true);
					}
				}
				else
					S9xReportButton(offset + i, true);
			}
			else
				S9xReportButton(offset + i, false);
		}
		// pointer
		offset = 0x80;
		UpdateCursorPosition(pad, cursor_x[0], cursor_y[0]);
		S9xReportPointer(offset, (u16) cursor_x[0], (u16) cursor_y[0]);
	}
	/*** Mouse ***/
	else if (Settings.MouseMaster && pad < GCSettings.Mouse)
	{
		// buttons
		offset = 0x60 + (2 * pad);
		for (i = 0; i < 2; i++)
		{
			if (jp & gcmousemap[i]
#ifdef HW_RVL
			|| wp & wmmousemap[i]
#endif
			)
				S9xReportButton(offset + i, true);
			else
				S9xReportButton(offset + i, false);
		}
		// pointer
		offset = 0x81;
		UpdateCursorPosition(pad, cursor_x[1 + pad], cursor_y[1 + pad]);
		S9xReportPointer(offset + pad, (u16) cursor_x[1 + pad],
				(u16) cursor_y[1 + pad]);
	}
	/*** Justifier ***/
	else if (Settings.JustifierMaster && pad < GCSettings.Justifier)
	{
		// buttons
		offset = 0x70 + (3 * pad);
		for (i = 0; i < 3; i++)
		{
			if (jp & gcjustmap[i]
#ifdef HW_RVL
			|| wp & wmjustmap[i]
#endif
			)
				S9xReportButton(offset + i, true);
			else
				S9xReportButton(offset + i, false);
		}
		// pointer
		offset = 0x83;
		UpdateCursorPosition(pad, cursor_x[3 + pad], cursor_y[3 + pad]);
		S9xReportPointer(offset + pad, (u16) cursor_x[3 + pad],
				(u16) cursor_y[3 + pad]);
	}
	
#ifdef HW_RVL
	// screenshot (temp)
	if (wp & CLASSIC_CTRL_BUTTON_ZR)
		S9xReportButton(0x90, true);
	else
		S9xReportButton(0x90, false);
#endif
}

/****************************************************************************
 * NGCReportButtons
 *
 * Called on each rendered frame
 * Our way of putting controller input into Snes9x
 ***************************************************************************/
void NGCReportButtons ()
{
	s8 gc_px = PAD_SubStickX (0);
	s8 gc_py = PAD_SubStickY (0);

	u16 gc_pb = PAD_ButtonsHeld (0);

#ifdef HW_RVL
	s8 wm_sx = WPAD_Stick (0,1,0);
	s8 wm_sy = WPAD_Stick (0,1,1);
	u32 wm_pb = WPAD_ButtonsHeld (0);	// wiimote / expansion button info
#endif


	/*** Check for video zoom ***/
	if (GCSettings.Zoom)
	{
		if (gc_py < -36 || gc_py > 36)
			zoom ((float) gc_py / -36);
#ifdef HW_RVL
		if (wm_sy < -36 || wm_sy > 36)
			zoom ((float) wm_sy / -36);
#endif
	}

    Settings.TurboMode = ( (gc_px > 70)
#ifdef HW_RVL
							|| (wm_sx > 70)
#endif
							);	// RIGHT on c-stick and on classic ctrlr right joystick

	/*** Check for menu:
	       CStick left
	       OR "L+R+X+Y" (eg. Hombrew/Adapted SNES controllers)
	       OR "Home" on the wiimote or classic controller
	       OR LEFT on classic right analog stick***/

    if ((gc_px < -70) ||
        ((gc_pb & PAD_TRIGGER_L) &&
         (gc_pb & PAD_TRIGGER_R ) &&
         (gc_pb & PAD_BUTTON_X) &&
         (gc_pb & PAD_BUTTON_Y ))
#ifdef HW_RVL
		 || (wm_pb & WPAD_BUTTON_HOME)
		 || (wm_pb & WPAD_CLASSIC_BUTTON_HOME)
#endif
       )
    {
        ConfigRequested = 1;	// go to the menu
    }
    else
    {
        int j = (Settings.MultiPlayer5Master == true ? 4 : 2);
        for (int i = 0; i < j; i++)
            decodepad (i);
    }
}

void SetControllers ()
{
	if (Settings.MultiPlayer5Master == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_MP5, 1, 2, 3, -1);
	}
	else if (Settings.SuperScopeMaster == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_SUPERSCOPE, 1, 0, 0, 0);
	}
	else if (Settings.MouseMaster == true)
	{
		S9xSetController (0, CTL_MOUSE, 0, 0, 0, 0);
		if (GCSettings.Mouse == 2)
			S9xSetController (1, CTL_MOUSE, 1, 0, 0, 0);
		else
			S9xSetController (1, CTL_JOYPAD, 1, 0, 0, 0);
	}
	else if (Settings.JustifierMaster == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		if(GCSettings.Justifier == 2)
			S9xSetController(1, CTL_JUSTIFIER, 1, 0, 0, 0);
		else
			S9xSetController(1, CTL_JUSTIFIER, 0, 0, 0, 0);
	}
	else
	{
		// Plugin 2 Joypads by default
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_JOYPAD, 1, 0, 0, 0);
	}
}

/****************************************************************************
 * Set the default mapping for NGC
 ***************************************************************************/
void SetDefaultButtonMap ()
{
  int maxcode = 0x10;
  s9xcommand_t cmd;

	/*** Joypad 1 ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 A");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 B");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 X");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Y");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 R");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Select");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Start");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Up");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Down");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Left");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad1 Right");

  maxcode = 0x20;
	/*** Joypad 2 ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 A");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 B");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 X");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Y");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 R");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Select");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Start");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Up");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Down");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Left");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad2 Right");

  maxcode = 0x30;
	/*** Joypad 3 ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 A");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 B");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 X");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Y");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 R");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Select");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Start");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Up");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Down");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Left");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad3 Right");

  maxcode = 0x40;
	/*** Joypad 4 ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 A");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 B");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 X");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Y");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 R");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Select");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Start");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Up");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Down");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Left");
  ASSIGN_BUTTON_FALSE (maxcode++, "Joypad4 Right");

  maxcode = 0x50;
	/*** Superscope ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope AimOffscreen");
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope Fire");
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope Cursor");
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope ToggleTurbo");
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope ToggleTurbo");
  ASSIGN_BUTTON_FALSE (maxcode++, "Superscope Pause");

  maxcode = 0x60;
	/*** Mouse ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Mouse1 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Mouse1 R");
  ASSIGN_BUTTON_FALSE (maxcode++, "Mouse2 L");
  ASSIGN_BUTTON_FALSE (maxcode++, "Mouse2 R");

  maxcode = 0x70;
	/*** Justifier ***/
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier1 AimOffscreen");
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier1 Trigger");
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier1 Start");
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier2 AimOffscreen");
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier2 Trigger");
  ASSIGN_BUTTON_FALSE (maxcode++, "Justifier2 Start");

  maxcode = 0x80;
  S9xMapPointer( maxcode++, S9xGetCommandT("Pointer Superscope"), false);
  S9xMapPointer( maxcode++, S9xGetCommandT("Pointer Mouse1"), false);
  S9xMapPointer( maxcode++, S9xGetCommandT("Pointer Mouse2"), false);
  S9xMapPointer( maxcode++, S9xGetCommandT("Pointer Justifier1"), false);
  S9xMapPointer( maxcode++, S9xGetCommandT("Pointer Justifier2"), false);
  
  maxcode = 0x90;
  ASSIGN_BUTTON_FALSE (maxcode++, "Screenshot");

  SetControllers ();

}

