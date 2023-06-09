﻿
#include "pch.h"
#include "framework.h"
#include "farseer3_mfc.h"
#include "farseer3_mfcDlg.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct infoLine
{
	glm::vec2 sp1, sp2;
	glm::vec3 colour;
	bool doDraw;
};

bool needTexUpdate = true;
int texW, texH, texCh;
//std::string texture_path = "test.png";
std::string texture_path = "sample.jpg";
unsigned char *tex_data;
unsigned int mTex;
int wWidth=800, wHeight=600;
int curX, curY;
float curXTrans, curYTrans;
float camX=0.0f, camY=0.0f;
float camScale=1.0;
glm::vec3 camera_pos = glm::vec3(0,0,-100);
Cfarseer3mfcDlg * dlgRef;

//operational controls
glm::vec2 p1(0.0f);
glm::vec2 p2(0.0f);

//automeasure controls
glm::vec2 tp1(0.0f), tp2(0.0f), tp3(0.0f), tp4(0.0f);
float amTol = 0.6f;
int amBS = 20;

vector<glm::vec2> measMarkers;

int opId = 0; //0 - nothing
			  //1 - scale x
			  //2 - scale y
			  //3 - measure
int cOp,cStage;
float scaleX = 1.0f, scaleY = 1.0f, nomX = 100.0f, nomY = 100.0f;
infoLine myIl = { glm::vec2(0.0f),glm::vec2(0.0f),glm::vec3(1.0f,0.0f,0.0f),false };
float lastMeasurement = 0.0;

flarr measJournal = {};

bool mDown = false;
bool mLeft = false;
bool mRight = false;
bool mMid = false;


// TFarseer3

BEGIN_MESSAGE_MAP(TFarseer3, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// app init method
TFarseer3::TFarseer3()
{
	std::thread oglThread(oglThreadFunc);
	oglThread.detach();
}


// TFarseer3 instance
TFarseer3 theApp;

BOOL TFarseer3::InitInstance()
{

	file_target = "sample.jpg";

	CWinApp::InitInstance();
	
	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Локальные приложения, созданные с помощью мастера приложений"));

	Cfarseer3mfcDlg dlg;

	dlgRef = &dlg;
	dlg.appRef = this;

	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Предупреждение. Не удалось создать диалоговое окно, поэтому работа приложения неожиданно завершена.\n");
		TRACE(traceAppMsg, 0, "Предупреждение. При использовании элементов управления MFC для диалогового окна невозможно #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

void TFarseer3::forceUpdateOglScales(bool byY)
{
	if (!byY)
	{
		scaleX = fScaleX;
		scaleY = fScaleX;
	}
	if (byY)
	{
		scaleX = fScaleY;
		scaleY = fScaleY;
	}
}

flarr TFarseer3::getMeasJournal()
{
	return measJournal;
}

//get distance
float getDist(glm::vec2 pt1, glm::vec2 pt2)
{
	return sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2));
}


float cbDelta(vector<glm::vec3> colourBuff)
{
	int n = 0;
	float delta = 0.0f;
	if (colourBuff.size() > 1)
	{
		float ccmn = 0.0f;
		float ccmx = 0.0f;
		for (glm::vec3 cc : colourBuff)
		{			
			float ccol = (cc.r + cc.g + cc.b) / 3.0f;
			if (n == 0) { ccmn = ccol; ccmx = ccol; }
			if (ccmn > ccol) ccmn = ccol;
			if (ccmx < ccol) ccmx = ccol;
			n++;
		}
		delta = abs(ccmx - ccmn);
		return delta;
	}
	else
		return 0.0f;
}



void autoMeasure(glm::vec2 pt1, glm::vec2 pt2, glm::vec2 pt3, glm::vec2 pt4, float tolerance, int crawlerBuffer)
{
	//get view direction from point 1 to point 2
	glm::vec2 axDir = glm::normalize(pt2 - pt1);

	//get view direction for crawler
	glm::vec2 crDir = glm::normalize(pt4 - pt3);

	//get expected crawler path lengths
	float axLen = glm::length(pt2 - pt1);
	float crLen = glm::length(pt4 - pt3);

	//position crawler at starting point
	glm::vec2 crStart = pt3;
	glm::vec2 cCrStart = pt3;
	glm::vec2 crPos = pt3;

	vector<glm::vec3> colourBuff = {}; //colour buffer for crawler
	vector<glm::vec2> deltaSet =   {}; //set of found deltas

	//all variables are now set, launch crawler
	float cCrPath = glm::length(cCrStart - crStart);
	float cLinePath = glm::length(cCrStart - crPos);
	
	flarr sizeSet = {};

	while (cCrPath < axLen)
	{
		cCrPath = glm::length(cCrStart - crStart);
		cLinePath = glm::length(cCrStart - crPos);
		deltaSet.clear();
		colourBuff.clear();
		while (cLinePath < crLen)
		{
			cLinePath = glm::length(cCrStart - crPos);
			glm::vec3 cClr = getDataPixels(tex_data, texCh, crPos.x, texH - crPos.y, texW, texH);
			colourBuff.push_back(cClr);
			//remove first element if the buffer is full
			if (colourBuff.size() > crawlerBuffer) colourBuff.erase(colourBuff.begin());
			float cDelta = cbDelta(colourBuff);
			if (cDelta > tolerance)
			{
				deltaSet.push_back(crPos - crDir * glm::vec2((float)(colourBuff.size()) / 2.0f));
				colourBuff.clear(); //clean up the buffer if delta is found
			}
			crPos += crDir;
		}
		if (deltaSet.size() == 0)
		{
			deltaSet.push_back(cCrStart);
			deltaSet.push_back(crPos);
		}
		if (deltaSet.size() == 1)
		{
			deltaSet.push_back(crPos);
		}

		glm::vec2 mpt1 = deltaSet[0];
		glm::vec2 mpt2 = deltaSet[deltaSet.size()-1];

		measMarkers.push_back(mpt1);
		measMarkers.push_back(mpt2);

		float dx = abs(mpt1.x - mpt2.x) * scaleX;
		float dy = abs(mpt1.y - mpt2.y) * scaleY;
		float measurement = sqrt(pow(dx, 2) + pow(dy, 2));
		sizeSet.push_back(measurement);

		cCrStart += axDir;
		crPos = cCrStart;
	}

	float sum = 0.0f;
	for (float cs : sizeSet)
	{
		sum += cs;
	}
	
	lastMeasurement = sum / (float)(sizeSet.size());

	measJournal.push_back(lastMeasurement);
	dlgRef->outpMeasJourn();

	CString outp;
	outp.Format(_T("Auto measured Value = %f"), lastMeasurement);

	if (dlgRef->vCbShowMessage.GetCheck())
		AfxMessageBox(outp, MB_OK | MB_ICONINFORMATION);

}

//select which manipulations to perform depending on
//current operation and stage of procedure
void processStage(int &op, int &stage, glm::vec2 &pt1, glm::vec2 &pt2, float cx, float cy)
{

	//get scale x
	if (op == 1)
	{
		if (stage == 0)
		{
			pt1.x = cx;
			pt1.y = cy;
			stage++;
			myIl.doDraw = true;
			myIl.sp1 = pt1;
		} else if (stage == 1)
		{
			pt2.x = cx;
			pt2.y = cy;
			stage=0;
			op = 0;
			theApp.demandOp = 0;
			scaleX = nomX / abs(pt1.x - pt2.x);
			myIl.doDraw = false;
			theApp.fScaleX = scaleX;
		}
	}

	//get scale y
	if (op == 2)
	{
		if (stage == 0)
		{
			pt1.x = cx;
			pt1.y = cy;
			stage++;
			myIl.doDraw = true;
			myIl.sp1 = pt1;
		}
		else if (stage == 1)
		{
			pt2.x = cx;
			pt2.y = cy;
			stage = 0;
			op = 0;
			theApp.demandOp = 0;
			scaleY = nomY / abs(pt1.y-pt2.y);
			myIl.doDraw = false;
			theApp.fScaleY = scaleY;
		}
	}

	//get measurement
	if (op == 3)
	{
		if (stage == 0)
		{
			pt1.x = cx;
			pt1.y = cy;
			stage++;
			myIl.doDraw = true;
			myIl.sp1 = pt1;
		}
		else if (stage == 1)
		{
			pt2.x = cx;
			pt2.y = cy;
			measMarkers.push_back(pt1);
			measMarkers.push_back(pt2);
			stage = 0;
			op = 0;
			theApp.demandOp = 0;
			float dx = abs(pt1.x - pt2.x) * scaleX;
			float dy = abs(pt1.y - pt2.y) * scaleY;
			lastMeasurement = sqrt(pow(dx, 2) + pow(dy, 2));
			measJournal.push_back(lastMeasurement);
			dlgRef->outpMeasJourn();

			CString outp;
			outp.Format(_T("Measured Value = %f"), lastMeasurement);

			if (dlgRef->vCbShowMessage.GetCheck())
			AfxMessageBox(outp,	MB_OK | MB_ICONINFORMATION);

			myIl.doDraw = false;
		}
	}


	//dropper
	if (op == 4)
	{
		if (stage == 0)
		{
			stage = 0;
			op = 0;
			theApp.demandOp = 0;

			//texture is inverted, so texH - cy
			glm::vec3 clr = getDataPixels(tex_data, texCh, cx, texH - cy, texW, texH);

			CString outp;
			outp.Format(_T("Pixel Colour at [%d;%d] = %f, %f, %f (%d, %d, %d)"), (int)cx, (int)cy, clr.r, clr.g, clr.b, (int)(clr.r*256.0f), (int)(clr.g*256.0f), (int)(clr.b*256.0f));
			AfxMessageBox(outp, MB_OK | MB_ICONINFORMATION);

			myIl.doDraw = false;
		}
	}

	//automeasure
	if (op == 5)
	{
		if (stage == 0)
		{
			tp1.x = cx;
			tp1.y = cy;
			stage++;
			myIl.doDraw = true;
			myIl.sp1 = tp1;
		}
		else if (stage == 1)
		{
			tp2.x = cx;
			tp2.y = cy;
			stage++;
			myIl.doDraw = false;
			myIl.sp1 = pt1;
		}
		else if (stage == 2)
		{
			tp3.x = cx;
			tp3.y = cy;
			stage++;
			myIl.doDraw = true;
			myIl.sp1 = tp3;
		}
		else if (stage == 3)
		{
			tp4.x = cx;
			tp4.y = cy;
			stage = 0;
			op = 0;
			theApp.demandOp = 0;

			autoMeasure(tp1, tp2, tp3, tp4,amTol,amBS);

			myIl.doDraw = false;
		}
	}

}


void makeMainTex(std::string file)
{
	delete(tex_data);
	tex_data = getImage(file, texW, texH, texCh);
	mTex = makeTexture(file);
}

void calcTransCurPos(int cx, int cy, int camx, int camy, 
					float sx, float sy, float &tx, float &ty)
{
	tx = camScale*cx + camx;
	ty = camScale*cy + camy;
}

void processMouseShift(int cx, int cy, int lx, int ly)
{
	if (mMid)
	{
		camX -= (float)(cx - lx);
		camY -= (float)(cy - ly);
	}
}

//win size callback
void window_size_callback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &width, &height);
	wWidth = width;
	wHeight = height;
}

//framebuffer callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &wWidth, &wHeight);
	glViewport(0, 0, wWidth, wHeight);
}


//cursor position callback
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int lCurX = curX;
	int lCurY = curY;
	curX = xpos;
	curY = wHeight-ypos;

	if (mDown) processMouseShift(curX,curY,lCurX,lCurY);

}

//mouse button click callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		mDown = true;
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			mMid = true;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			mLeft = true;
			processStage(cOp,cStage,p1,p2,curXTrans,curYTrans);
		}
	}
	else
	{
		mDown = false;
	}
}

//close callback
void window_close_callback(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, GLFW_FALSE);
}

//scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float k = 1.0f;
	float delta = 1.0f;

	if (yoffset > 0) k = .2f;
	if (yoffset < 0) k = -.2f;
	if (yoffset == 0) k = 0.0f;

	camScale += delta * k;
	if (camScale < 0.2f) camScale = 0.2f;
}


std::string CStringToStr(CString inp)
{
	CT2CA pszConvertedAnsiString(inp);
	std::string fts(pszConvertedAnsiString);
	return fts;
}


//main thread func
void oglThreadFunc()
{
	OGLManager oMan(wWidth, wHeight, window_size_callback);
	glfwSetCursorPosCallback(oMan.window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(oMan.window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(oMan.window, mouse_button_callback);
	glfwSetWindowCloseCallback(oMan.window, window_close_callback);
	glfwSetScrollCallback(oMan.window, scroll_callback);

	oMan.addShader("shader_chart_vert.gls", "shader_chart_frag.gls");
	oMan.addShader("shader_simple_vert.gls", "shader_simple_frag.gls");

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(oMan.window))
	{
		std::string fts = CStringToStr(theApp.file_target);

		if (texture_path != fts)
		{
			texture_path = fts;
			needTexUpdate = true;
		}

		if (needTexUpdate)
		{
			needTexUpdate = false;
			makeMainTex(texture_path);
		}

		if (theApp.demandOp != cOp)
		{
			cOp = theApp.demandOp;
			cStage = 0;
			if ((cOp == 1) || (cOp == 2))
			{
				nomX = theApp.nomSizeX;
				nomY = theApp.nomSizeY;
			}
			if (cOp == 5)
			{
				amTol = theApp.expTolerance;
				amBS =  theApp.expBufSize;
			}
		}

		//SetDlgItemText((HWND)IDC_STATIC1, 0, L"Hello");
		theApp.ww = wWidth;
		theApp.wh = wHeight;
		
		calcTransCurPos(curX, curY, camX, camY, 1.0f, 1.0f, curXTrans, curYTrans);

		theApp.oglCurX = curXTrans;
		theApp.oglCurY = curYTrans;

		dlgRef->updateOglState();
        
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glfwGetWindowSize(oMan.window, &wWidth, &wHeight);

		glm::vec3 cam_pos(camX, camY, 0.0f);
		glm::mat4 mat_persp = glm::ortho(0.0f, (float)wWidth * camScale, 0.0f, (float)wHeight * camScale,-100.0f,100.0f);
		glm::mat4 mat_view = glm::lookAt(glm::vec3(cam_pos.x, cam_pos.y, 0.0f),glm::vec3(cam_pos.x, cam_pos.y, -100.0f),glm::vec3(0.0f,1.0f,0.0f));

		//set uniforms for shader
		oMan.setDefaultProjections();
		oMan.setView(mat_view);
		oMan.setProjection(mat_persp);
		oMan.updateProjectionForShader(0);
		oMan.updateProjectionForShader(1);

		drawPlaneOrigin(oMan.getShader(0), glm::vec3(0.0f), texW, texH, glm::vec3(1.0f,1.0f,1.0f), mTex, true);

		for (glm::vec2 mp : measMarkers)
		{
			drawPlaneOrigin(oMan.getShader(1), glm::vec3(mp.x,mp.y,1.5f), 5.0f, 5.0f, glm::vec3(0.0f, 0.0f, 1.0f), 0, false);
		}
		while (measMarkers.size() > 200)
		{
			measMarkers.erase(measMarkers.begin());
		}

		if (myIl.doDraw)
		{
			glm::vec3 ilp1(myIl.sp1.x, myIl.sp1.y, 1.0f);
			glm::vec3 ilp2(myIl.sp2.x, myIl.sp2.y, 1.0f);
			drawLine(oMan.getShader(1), ilp1, glm::vec3(curXTrans, curYTrans, 1.0f), myIl.colour);
		}

		oMan.endDraw();
	}
}