#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		
#include <thread>

#include "ourGraphics.h"
#include "ourGraphicsFreeType.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/perpendicular.hpp>

void oglThreadFunc();
void window_size_callback(GLFWwindow* window, int width, int height);


class TFarseer3 : public CWinApp
{
public:
	TFarseer3();
	int ww, wh;
	int oglCurX, oglCurY;
	CString file_target;
	float nomSizeX, nomSizeY, fScaleX, fScaleY;
	float expTolerance;
	int expBufSize;
	int expNSteps;
		
	int demandOp;

	void forceUpdateOglScales(bool byY);
	flarr getMeasJournal();
	void clearMeasJournal();

public:
	virtual BOOL InitInstance();


	DECLARE_MESSAGE_MAP()
};

extern TFarseer3 theApp;
