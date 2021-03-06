#include "PainterEngine_MessageBox.h"


px_void PX_MessageBox_BtnYesClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	PX_MessageBox *pm=(PX_MessageBox *)user;
	pm->retVal=PX_MESSAGEBOX_RETURN_YES;
	pm->mode=PX_MESSAGEBOX_MODE_CLOSE;
	if (pm->function_yes)
	{
		pm->function_yes(pm->function_yes_ptr);
	}

}

px_void PX_MessageBox_BtnNoClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	PX_MessageBox *pm=(PX_MessageBox *)user;
	pm->retVal=PX_MESSAGEBOX_RETURN_NO;
	pm->mode=PX_MESSAGEBOX_MODE_CLOSE;
	if (pm->function_no)
	{
		pm->function_no(pm->function_yes_ptr);
	}
}


px_bool PX_MessageBoxInitialize(PX_Runtime *runtime,PX_MessageBox *pm,PX_FontModule *fontmodule,px_int window_Width,px_int window_Height)
{

	pm->schedule=0;
	pm->runtime=runtime;
	pm->window_Width=window_Width;
	pm->window_Height=window_Height;
	pm->retVal=PX_MESSAGEBOX_RETURN_NONE;
	pm->function_no=PX_NULL;
	pm->function_yes=PX_NULL;

	if(!(pm->root=PX_ObjectCreate(&runtime->mp_ui,PX_NULL,0,0,0,0,0,0))) return PX_FALSE;
	
	if(!(pm->btn_Ok=PX_Object_PushButtonCreate(&runtime->mp_ui,pm->root,window_Width/2+200,window_Height/2+150,84,28,"OK",PX_NULL,PX_COLOR(255,0,0,0)))) return PX_FALSE;
	
	if(!(pm->btn_Cancel=PX_Object_PushButtonCreate(&runtime->mp_ui,pm->root,window_Width/2+300,window_Height/2+150,84,28,"Cancel",PX_NULL,PX_COLOR(255,0,0,0)))) return PX_FALSE;
	

	pm->Message=PX_NULL;
	pm->show=PX_FALSE;
	pm->fontmodule=fontmodule;

	pm->PX_MESSAGEBOX_STAGE_1_HEIGHT=PX_MESSAGEBOX_DEFAULT_STAGE_1_HEIGHT;
	pm->PX_MESSAGEBOX_STAGE_2_HEIGHT=PX_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT;

	PX_ObjectRegisterEvent(pm->btn_Ok,PX_OBJECT_EVENT_EXECUTE,PX_MessageBox_BtnYesClick,pm);
	PX_ObjectRegisterEvent(pm->btn_Cancel,PX_OBJECT_EVENT_EXECUTE,PX_MessageBox_BtnNoClick,pm);
	return PX_TRUE;
}

px_void PX_MessageBoxPostEvent(PX_MessageBox *pm,PX_Object_Event e)
{
	if (pm->show)
	{
		PX_ObjectPostEvent(pm->root,e);
	}
}

px_void PX_MessageBoxUpdate(PX_MessageBox *pm,px_dword elpased)
{
	if (elpased>2000)
	{
		return;
	}
	if (pm->show)
	{
		PX_ObjectUpdate(pm->root,elpased);
		switch(pm->mode)
		{
		case PX_MESSAGEBOX_MODE_EXPAND:
			pm->schedule+=elpased;
			if (pm->schedule>PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME)
			{
				pm->schedule=PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME;
			}
			break;
		case PX_MESSAGEBOX_MODE_CLOSE:
		default:
			{
				pm->schedule-=elpased;
				if (pm->schedule<0)
				{
					pm->schedule=0;
					pm->show=PX_FALSE;
				}
			}
			break;

		}
		
		
	}
}

px_void PX_MessageBoxClose(PX_MessageBox *pm)
{
	pm->mode=PX_MESSAGEBOX_MODE_CLOSE;
}

px_void PX_MessageBoxRender(px_surface *pSurface,PX_MessageBox *pm,px_dword elpased)
{
	px_color backGroundColor,frontColor;
	if (elpased>2000)
	{
		return;
	}

	if (!pm->show)
	{
	   return;
	}

	switch(pm->colormod)
	{
	case PX_MESSAGEBOX_COLORMOD_LIGHT:
		{
			backGroundColor=PX_COLOR(255,0,0,0);
			frontColor=PX_COLOR(255,255,255,255);
		}
		break;
	case PX_MESSAGEBOX_COLORMOD_NIGHT:
	default:
		{
			backGroundColor=PX_COLOR(255,255,255,255);
			frontColor=PX_COLOR(255,0,0,0);
		}
		break;
	}

	

	if (pm->schedule<PX_MESSAGEBOX_DEFAULT_STAGE_1_TIME)
	{
		px_int width=pm->window_Width*pm->schedule*pm->schedule/(PX_MESSAGEBOX_DEFAULT_STAGE_1_TIME*PX_MESSAGEBOX_DEFAULT_STAGE_1_TIME);
		PX_GeoDrawRect(pSurface,(pm->window_Width-width)/2,(pm->window_Height-pm->PX_MESSAGEBOX_STAGE_1_HEIGHT)/2,(pm->window_Width+width)/2,(pm->window_Height+pm->PX_MESSAGEBOX_STAGE_1_HEIGHT)/2,backGroundColor);
	}
	else if(pm->schedule<PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME)
	{
		px_int height=PX_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT*pm->schedule*pm->schedule/(PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME*PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME);
		PX_GeoDrawRect(pSurface,0,(pm->window_Height-height)/2,pm->window_Width-1,(pm->window_Height+height)/2,backGroundColor);
	}
	else
	{
		PX_GeoDrawRect(pSurface,0,(pm->window_Height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2,pm->window_Width-1,(pm->window_Height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2,backGroundColor);
		PX_GeoDrawRect(pSurface,0,(pm->window_Height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2-30,pm->window_Width-1,(pm->window_Height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2-10,backGroundColor);
		PX_GeoDrawRect(pSurface,0,(pm->window_Height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2+10,pm->window_Width-1,(pm->window_Height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2+30,backGroundColor);

		if (pm->fontmodule)
		{
			PX_FontModuleDrawText(pSurface,pm->fontmodule,pm->window_Width/2,pm->window_Height/2,PX_FONT_ALIGN_CENTER,pm->Message,frontColor);
		}
		else
		{
			PX_FontDrawText(pSurface,pm->window_Width/2,pm->window_Height/2,PX_FONT_ALIGN_CENTER,pm->Message,frontColor);
		}

		PX_ObjectRender(pSurface,pm->root,elpased);
	}

	
}

px_void PX_MessageBoxAlertOk(PX_MessageBox *pm,const px_char *message)
{
	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->Message=message;
	pm->mode=PX_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_FALSE;
	pm->btn_Ok->Visible=PX_TRUE;

	pm->function_yes=PX_NULL;
	pm->function_no_ptr=PX_NULL;
}

px_void PX_MessageBoxAlertOkEx(PX_MessageBox *pm,const px_char *message,PX_MessageBoxCallBack func_callback,px_void *ptr)
{
	pm->function_yes=func_callback;
	pm->function_yes_ptr=ptr;

	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->Message=message;
	pm->mode=PX_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_FALSE;
	pm->btn_Ok->Visible=PX_TRUE;;
}

px_void PX_MessageBoxAlert(PX_MessageBox *pm,const px_char *message)
{
	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->Message=message;
	pm->mode=PX_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_FALSE;
	pm->btn_Ok->Visible=PX_FALSE;

	pm->function_yes=PX_NULL;
	pm->function_no_ptr=PX_NULL;
}



px_void PX_MessageBoxAlertYesNo(PX_MessageBox *pm,const char *message)
{
	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->Message=message;
	pm->mode=PX_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_TRUE;
	pm->btn_Ok->Visible=PX_TRUE;
	pm->function_yes=PX_NULL;
	pm->function_no_ptr=PX_NULL;
}


px_void PX_MessageBoxAlertYesNoEx(PX_MessageBox *pm,const char *Message,PX_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_MessageBoxCallBack func_nocallback,px_void *noptr)
{
	pm->function_yes=func_yescallback;
	pm->function_no=func_nocallback;
	pm->function_yes_ptr=yesptr;
	pm->function_no_ptr=noptr;

	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->Message=Message;
	pm->mode=PX_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_TRUE;
	pm->btn_Ok->Visible=PX_TRUE;
}

px_void PX_MessageBoxSetColorModule(PX_MessageBox *pm,PX_MESSAGEBOX_COLORMOD colormod)
{
	pm->colormod=colormod;

	switch(pm->colormod)
	{
	case PX_MESSAGEBOX_COLORMOD_LIGHT:
		{
			PX_Object_PushButtonSetTextColor(pm->btn_Ok,PX_COLOR(255,255,255,255));
			PX_Object_PushButtonSetBackgroundColor(pm->btn_Ok,PX_COLOR(255,0,0,0));
			PX_Object_PushButtonSetCursorColor(pm->btn_Ok,PX_COLOR(255,192,192,192));
			PX_Object_PushButtonSetPushColor(pm->btn_Ok,PX_COLOR(255,96,96,96));
			PX_Object_PushButtonSetBorderColor(pm->btn_Ok,PX_COLOR(255,128,128,128));
			PX_Object_PushButtonSetTextColor(pm->btn_Cancel,PX_COLOR(255,255,255,255));
			PX_Object_PushButtonSetBackgroundColor(pm->btn_Cancel,PX_COLOR(255,0,0,0));
			PX_Object_PushButtonSetCursorColor(pm->btn_Cancel,PX_COLOR(255,192,192,192));
			PX_Object_PushButtonSetPushColor(pm->btn_Cancel,PX_COLOR(255,96,96,96));
			PX_Object_PushButtonSetBorderColor(pm->btn_Cancel,PX_COLOR(255,128,128,128));
		}
		break;
	case PX_MESSAGEBOX_COLORMOD_NIGHT:
	default:
		{
			PX_Object_PushButtonSetTextColor(pm->btn_Ok,PX_COLOR(255,0,0,0));
			PX_Object_PushButtonSetBackgroundColor(pm->btn_Ok,PX_COLOR(255,255,255,255));
			PX_Object_PushButtonSetCursorColor(pm->btn_Ok,PX_COLOR(255,192,192,192));
			PX_Object_PushButtonSetPushColor(pm->btn_Ok,PX_COLOR(255,96,96,96));
			PX_Object_PushButtonSetBorderColor(pm->btn_Ok,PX_COLOR(255,128,128,128));
			PX_Object_PushButtonSetTextColor(pm->btn_Cancel,PX_COLOR(255,0,0,0));
			PX_Object_PushButtonSetBackgroundColor(pm->btn_Cancel,PX_COLOR(255,255,255,255));
			PX_Object_PushButtonSetCursorColor(pm->btn_Cancel,PX_COLOR(255,192,192,192));
			PX_Object_PushButtonSetPushColor(pm->btn_Cancel,PX_COLOR(255,96,96,96));
			PX_Object_PushButtonSetBorderColor(pm->btn_Cancel,PX_COLOR(255,128,128,128));
		}
		break;
	}

}

px_void PX_MessageBoxSetFontModule(PX_MessageBox *pm,PX_FontModule *fm)
{
	pm->fontmodule=fm;
}

PX_MESSAGEBOX_RETURN PX_MessageBoxGetLastReturn(PX_MessageBox *pm)
{
	PX_MESSAGEBOX_RETURN ret=pm->retVal;
	pm->retVal=PX_MESSAGEBOX_RETURN_NONE;
	return ret;
}
