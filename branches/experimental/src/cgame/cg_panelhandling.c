/*
 * name:		cg_panelhandling.c
 *
 * desc:		Panel Handling
 *
 */

#include "cg_local.h"

// ======================================================

panel_button_t* cg_focusButton;

/*
================
CG_PanelButton_RenderEdit
================
*/
void CG_PanelButton_RenderEdit( panel_button_t* button ) {
	qboolean useCvar = button->data[0] ? qfalse : qtrue;
	int offset = -1;

	if( useCvar ) {
		char buffer[256 + 1];
		trap_Cvar_VariableStringBuffer( button->text, buffer, sizeof(buffer) );

		if( cg_focusButton == button && ((cg.time / 1000) % 2)) {
			if( trap_Key_GetOverstrikeMode() )
				Q_strcat( buffer, sizeof(buffer), "^0|" );
			else
				Q_strcat( buffer, sizeof(buffer), "^0_" );
		}
		else {
			Q_strcat( buffer, sizeof(buffer), " " );
		}

		do {
			offset++;
			if( buffer + offset  == '\0' )
				break;
		} while( CG_Text_Width_Ext( buffer + offset, button->font->scalex, 0, button->font->font ) > button->rect.w );

		CG_Text_Paint_Ext( button->rect.x, button->rect.y + button->rect.h, button->font->scalex, button->font->scaley, button->font->colour, va( "^7%s", buffer + offset ), 0, 0, button->font->style, button->font->font );
	}
	else {
		int maxlen = button->data[0];
		char *s;

		if( cg_focusButton == button && ((cg.time / 1000) % 2)) {
			if( trap_Key_GetOverstrikeMode() )
				s = va( "^7%s^0|", button->text );
			else
				s = va( "^7%s^0_", button->text );
		}
		else {
			s = va( "^7%s ", button->text );	// space hack to make the text not blink
		}

		do {
			offset++;
			if( s + offset  == '\0' )
				break;
		} while( CG_Text_Width_Ext( s + offset, button->font->scalex, 0, button->font->font ) > button->rect.w );

		CG_Text_Paint_Ext( button->rect.x, button->rect.y + button->rect.h, button->font->scalex, button->font->scaley, button->font->colour, s + offset, 0, 0, button->font->style, button->font->font );
	}
}

/*
================
CG_PanelButton_EditClick
================
*/
qboolean CG_PanelButton_EditClick( panel_button_t* button, int key ) {
	if( key == K_MOUSE1 ) {
		if( !CG_CursorInRect( &button->rect ) && cg_focusButton == button ) {
			CG_PanelButtons_SetFocusButton( NULL );
			if( button->onFinish ) {
				button->onFinish( button );
			}
			return qfalse;
		}
		else {
			CG_PanelButtons_SetFocusButton( button );
			return qtrue;
		}
	}
	else {
		char buffer[256];
		char *s;
		int len, maxlen;
		qboolean useCvar = button->data[0] ? qfalse : qtrue;

		if( useCvar ) {
			maxlen = sizeof(buffer);
			trap_Cvar_VariableStringBuffer( button->text, buffer, sizeof(buffer) );
			len = strlen( buffer );
		}
		else {
			maxlen = button->data[0];
			s = (char *)button->text;
			len = strlen( s );
		}

		if( key & K_CHAR_FLAG ) {
			key &= ~K_CHAR_FLAG;

			if( key == 'h' - 'a' + 1 )	{	// ctrl-h is backspace
				if( len ) {
					if( useCvar ) {
						buffer[len-1] = '\0';
						trap_Cvar_Set( button->text, buffer );
					} else {
						s[len-1] = '\0';
					}
				}
	    		return qtrue;
			}

			if( key < 32 ) {
			    return qtrue;
		    }

			if( len >= maxlen - 1 ) {
				return qtrue;
			}

			if( useCvar ) {
				buffer[len] = key;
				buffer[len+1] = '\0';
				trap_Cvar_Set( button->text, buffer );
			} else {
				s[len] = key;
				s[len+1] = '\0';
			}
			return qtrue;
		}
		else {
			if( key == K_ENTER || key == K_KP_ENTER ) {
				if( button->onFinish ) {
					button->onFinish( button );
				}
			}
		}
	}

	return qtrue;
}

/*
================
CG_PanelButtonsKeyEvent
================
*/
qboolean CG_PanelButtonsKeyEvent( int key, qboolean down, panel_button_t** buttons ) {
	panel_button_t* button;

	if( cg_focusButton ) {
		for( ; *buttons; buttons++ ) {
			button = (*buttons);

			if( button == cg_focusButton ) {
				if( button->onKeyDown && down ) {
					if( !button->onKeyDown( button, key ) ) {
						if( cg_focusButton ) {
							return qfalse;
						}
					}
				}
				if( button->onKeyUp && !down ) {
					if( !button->onKeyUp( button, key ) ) {
						if( cg_focusButton ) {
							return qfalse;
						}
					}
				}
			}
		}
	}

	if( down ) {
		for( ; *buttons; buttons++ ) {
			button = (*buttons);

			if( button->onKeyDown ) {
				if( CG_CursorInRect( &button->rect ) ) {
					if( button->onKeyDown( button, key ) ) {
						return qtrue;
					}
				}
			}
		}
	}
	else {
		for( ; *buttons; buttons++ ) {
			button = (*buttons);

			if( button->onKeyUp && CG_CursorInRect( &button->rect )) {
				if( button->onKeyUp( button, key ) ) {
					return qtrue;
				}
			}
		}
	}

	return qfalse;
}

/*
================
CG_PanelButtonsSetup
================
*/
void CG_PanelButtonsSetup( panel_button_t** buttons ) {
	panel_button_t* button;

	for( ; *buttons; buttons++ ) {
		button = (*buttons);

		if( button->shaderNormal ) {
			button->hShaderNormal = trap_R_RegisterShaderNoMip( button->shaderNormal );
		}
	}
}

/*
================
CG_PanelButtonsRender
================
*/
void CG_PanelButtonsRender( panel_button_t** buttons ) {
	panel_button_t* button;

	for( ; *buttons; buttons++ ) {
		button = (*buttons);

		if( button->onDraw ) {
			button->onDraw( button );
		}
	}
}

/*
================
CG_PanelButtonsRender_Text
================
*/
void CG_PanelButtonsRender_Text( panel_button_t* button ) {
	if( !button->font ) {
		return;
	}

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, button->text, 0, 0, button->font->style, button->font->font );
}

/*
================
CG_PanelButtonsRender_Img
================
*/
void CG_PanelButtonsRender_Img( panel_button_t* button ) {
	CG_DrawPic( button->rect.x, button->rect.y, button->rect.w, button->rect.h, button->hShaderNormal );
}

/*
================
CG_PanelButtons_GetFocusButton
================
*/
panel_button_t* CG_PanelButtons_GetFocusButton( void ) {
	return cg_focusButton;
}

/*
================
CG_PanelButtons_SetFocusButton
================
*/
void CG_PanelButtons_SetFocusButton( panel_button_t* button ) {
	cg_focusButton = button;
}
