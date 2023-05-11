
// farseer3_mfc.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// основные символы
#include <thread>

#include "ourGraphics.h"
#include "ourGraphicsFreeType.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void oglThreadFunc();
void window_size_callback(GLFWwindow* window, int width, int height);


// TFarseer3:
// Сведения о реализации этого класса: farseer3_mfc.cpp
//

class TFarseer3 : public CWinApp
{
public:
	TFarseer3();
	int ww, wh;
	int oglCurX, oglCurY;
	CString file_target;
	float nomSizeX, nomSizeY, fScaleX, fScaleY;
		
	int demandOp;

	void forceUpdateOglScales(bool byY);
	flarr getMeasJournal();


// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern TFarseer3 theApp;
