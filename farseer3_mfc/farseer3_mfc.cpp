
#include "pch.h"
#include "framework.h"
#include "farseer3_mfc.h"
#include "farseer3_mfcDlg.h"
#include <string>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include "MinimumAreaBox2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//rectangle definition
struct rectangle
{
	glm::vec2 center, axis1, axis2;
	glm::vec2 extent, extentScaled;
	float area;
};

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
int amNSteps = 20;
rectangle resultingRect;

vector<glm::vec2> measMarkers;
vector<glm::vec2> polyPoints;
vector<glm::vec2> globalDeltaSet;

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

//forward declarations
void processPointSet(vector<glm::vec2> &dataPts);
gte::OrientedBox2<float> getMAR(vector<glm::vec2> convHull);
rectangle getBoundingBox();


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

	SetRegistryKey(_T("Local app"));

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
		TRACE(traceAppMsg, 0, "Error making dialog window, quitting.\n");
		TRACE(traceAppMsg, 0, "Error adding MFC dialog controls. Imossible to #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
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

void TFarseer3::clearMeasJournal()
{
	measJournal.clear();
}

//get distance
float getDist(glm::vec2 pt1, glm::vec2 pt2)
{
	return sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2));
}

float colAvg(glm::vec3 col)
{
	return (col.r + col.g + col.b) / 3.0f;
}

float cbDelta(vector<glm::vec3> colourBuff)
{
	int n = 0;
	float delta = 0.0f;
	if (colourBuff.size() > 1)
	{
		float ccmn = colAvg(colourBuff[0]);
		float ccmx = colAvg(colourBuff[0]);
		for (glm::vec3 cc : colourBuff)
		{			
			float ccol = colAvg(cc);
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

void autoWeighDeltas(vector<glm::vec2> &ds, vector<float> &dsc)
{
	int l = ds.size();
	float sum = 0.0f;
	float avgD=0.0f;
	for (int i = 0; i < ds.size() - 1; i++)
	{
		sum += glm::length(ds[i] - ds[i + 1]);
	}
	avgD = sum / (float)ds.size();

	bool opDone = false;

	do
	{
		opDone = false;
		for (int i = 0; i < ds.size() - 1; i++)
		{
			if (glm::length(ds[i] - ds[i + 1]) < avgD)
			{
				ds[i] = (ds[i] + ds[i + 1]) / 2.0f;
				ds.erase(ds.begin() + i + 1);
				dsc[i] = (dsc[i] + dsc[i + 1]) / 2.0f;
				dsc.erase(dsc.begin() + i + 1);
				opDone = true;
			}
		}
	} while (opDone);
}

void autoMeasure(glm::vec2 pt1, glm::vec2 pt2, glm::vec2 pt3, glm::vec2 pt4, float tolerance, int crawlerBuffer, int nSteps)
{
	//clear mesurement markers
	measMarkers.clear();
	
	//get view direction from point 1 to point 2
	glm::vec2 axDir = glm::normalize(pt2 - pt1);

	//get view direction for crawler
	glm::vec2 crDir = glm::normalize(pt4 - pt3);

	//get expected crawler path lengths
	float axLen = glm::length(pt2 - pt1);
	float axStep = axLen / (float)nSteps;
	float crLen = glm::length(pt4 - pt3);

	//position crawler at starting point
	glm::vec2 crStart = pt3;
	glm::vec2 cCrStart = pt3;
	glm::vec2 crPos = pt3;

	vector<glm::vec3> colourBuff = {}; //colour buffer for crawler
	vector<glm::vec2> deltaSet =   {}; //set of found deltas
	vector<float> deltaSetColours = {}; //colour buffer for crawler

	globalDeltaSet.clear();
	polyPoints.clear();

	//all variables are now set, launch crawler
	float cCrPath = glm::length(cCrStart - crStart);
	float cLinePath = glm::length(cCrStart - crPos);
	
	flarr sizeSet = {};

	while (cCrPath < axLen)
	{
		cCrPath = glm::length(cCrStart - crStart);
		cLinePath = glm::length(cCrStart - crPos);
		deltaSet.clear();
		deltaSetColours.clear();
		colourBuff.clear();
		while (cLinePath < crLen)
		{
			cLinePath = glm::length(cCrStart - crPos);
			glm::vec3 cClr = getDataPixels(tex_data, texCh, crPos.x, texH - crPos.y, texW, texH);
			colourBuff.push_back(cClr);
			//remove first element if the buffer is full
			if (colourBuff.size() > crawlerBuffer) colourBuff.erase(colourBuff.begin());
			if (colourBuff.size() == crawlerBuffer)
			{
				float cDelta = cbDelta(colourBuff);
				if (cDelta > tolerance)
				{
					deltaSet.push_back(crPos - crDir * glm::vec2((float)(colourBuff.size()) / 2.0f));
					deltaSetColours.push_back(cDelta);
					colourBuff.clear(); //clean up the buffer if delta is found
					crPos += crDir*(float)colourBuff.size()- crDir;
				}
			}
			crPos += crDir;
		}

		if (deltaSet.size() > 1)
		{			

			//autoWeighDeltas(deltaSet, deltaSetColours);

			for (glm::vec2 pt : deltaSet)
			{
				globalDeltaSet.push_back(pt);
			}

			float maxDelta1 = deltaSetColours[0];
			float maxDelta2 = deltaSetColours[deltaSetColours.size() - 1];
			int id1=0, id2= deltaSetColours.size() - 1;
			for (int ii = 0; ii < deltaSetColours.size()/2; ii++)
			{
				if (maxDelta1 < deltaSetColours[ii])
				{
					maxDelta1 = deltaSetColours[ii];
					id1 = ii;
				}
			}
			for (int ii = deltaSetColours.size()-1; ii > id1; ii--)
			{
				if (maxDelta2 < deltaSetColours[ii])
				{
					maxDelta2 = deltaSetColours[ii];
					id2 = ii;
				}
			}


			if (id1 > id2)
			{
				int bf = id2; id2 = id1; id1 = bf;
			}

			glm::vec2 mpt1 = deltaSet[id1];
			glm::vec2 mpt2 = deltaSet[id2];
			
			measMarkers.push_back(mpt1);
			measMarkers.push_back(mpt2);
		}
		
		cCrStart += axDir*axStep;
		crPos = cCrStart;
	}

	//remove extreme values
	processPointSet(measMarkers);

	resultingRect = getBoundingBox();

	//scale values
	
	int ssl = measMarkers.size();

	for (int ii=0;ii<ssl;ii+=2)
	{
		float dx = abs(measMarkers[ii+1].x - measMarkers[ii].x) * scaleX;
		float dy = abs(measMarkers[ii+1].y - measMarkers[ii].y) * scaleY;
		float measurement = sqrt(pow(dx, 2) + pow(dy, 2));
		sizeSet.push_back(measurement);
	}

	resultingRect.extentScaled.x = resultingRect.extent.x * scaleX;
	resultingRect.extentScaled.y = resultingRect.extent.y * scaleY;

	//process size set
	float sum = 0.0f;
	for (float cs : sizeSet)
	{
		sum += cs;
	}
	
	if (sizeSet.size() > 0)
	{
		lastMeasurement = sum / (float)(sizeSet.size());

		measJournal.push_back(lastMeasurement);
		measJournal.push_back(resultingRect.extentScaled.x*2);
		measJournal.push_back(resultingRect.extentScaled.y*2);
		
	}
	else
	{
		lastMeasurement = -1;
		measJournal.push_back(-1);
		measJournal.push_back(-1);
		measJournal.push_back(-1);
	}

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

			autoMeasure(tp1, tp2, tp3, tp4,amTol,amBS,amNSteps);

			myIl.doDraw = false;
		}
	}

}

void processPointSet(vector<glm::vec2> &dataPts)
{
	if (dataPts.size() > 4)
	{

		int cIPoint = 0;
		float minDeltaId = 0, minDelta = 0;
		float maxDeltaId = 0, maxDelta = 0;
		for (int i = 0; i < 2; i++)
		{
			if (dataPts.size() > 6)
			{
				minDeltaId = 0;
				minDelta = glm::length(dataPts[1] - dataPts[0]);
				while (cIPoint < dataPts.size() - 1)
				{
					float cPairLength = glm::length(dataPts[cIPoint + 1] - dataPts[cIPoint]);
					if ((cPairLength < minDelta) && (i == 0))
					{
						minDeltaId = cIPoint;
						minDelta = cPairLength;

					}
					if ((cPairLength > maxDelta) && (i == 1))
					{
						maxDeltaId = cIPoint;
						maxDelta = cPairLength;
					}
					cIPoint += 2;
				}
			}
			if (i == 0)
			{
				dataPts.erase(dataPts.begin() + minDeltaId);
				dataPts.erase(dataPts.begin() + minDeltaId);
			}
			if (i == 1)
			{
				dataPts.erase(dataPts.begin() + maxDeltaId);
				dataPts.erase(dataPts.begin() + maxDeltaId);
			}
		}
	}
}

using MABRational = double;

gte::OrientedBox2<float> getMAR(vector<glm::vec2> convHull)
{
	gte::MinimumAreaBox2<float, MABRational> mab{};
	const int a = convHull.size();
	vector<gte::Vector2<float>> pts = {};

	for (glm::vec2 pt : convHull)
	{
		gte::Vector2<float> cpt;
		cpt[0] = pt.x;
		cpt[1] = pt.y;
		pts.push_back(cpt);
	}

	return mab(a,&pts[0]);
}


//calc bounding rectangle
rectangle getBoundingBox()
{
	
	if (measMarkers.size() > 4)
	{
		typedef boost::geometry::model::d2::point_xy<double> point;

		boost::geometry::model::polygon<point> polygon;

		string polyExpression = "";

		polyPoints.clear();

		int counter = 0;
		int ir = 0;
		int dir = 1;
		bool swapped = false;
		while (counter < measMarkers.size())
		{
			if (ir >= 0 && ir < measMarkers.size())
			{
				string cp_add = "";
				if (counter != 0) cp_add = ",";

				glm::vec2 mp = measMarkers[ir];				

				string cp = cp_add + to_string(mp.x) + " " + to_string(mp.y);
				polyExpression += cp;
				counter++;

				ir += 2 * dir;

				if ((ir >= measMarkers.size()) && (!swapped)) {
					ir--; dir = -1; swapped = true;
				}
			}
			else break;
		}

		string polyExpressionFull = "POLYGON((" + polyExpression + "))";

		boost::geometry::read_wkt(polyExpressionFull.c_str(), polygon);

		boost::geometry::model::polygon<point> pHull;
		boost::geometry::convex_hull(polygon, pHull);

		//turn convex hull to a regular vector
		vector<glm::vec2> hullPoints = {};
		for (const auto& point : pHull.outer())
		{
			float x = point.x();
			float y = point.y();
			hullPoints.push_back(glm::vec2(x, y));
			polyPoints.push_back(glm::vec2(x, y));
		}

		//get rectangle
		rectangle minRect;
		
		gte::OrientedBox2<float> myMAR = getMAR(hullPoints);
		minRect.extent = { myMAR.extent[0], myMAR.extent[1] };
		minRect.center = { myMAR.center[0], myMAR.center[1] };
		minRect.axis1 = { myMAR.axis[0][0], myMAR.axis[0][1] };
		minRect.axis2 = { myMAR.axis[1][0], myMAR.axis[1][1] };		

		return minRect;
	}
	else return { {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},-1 };
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
				amNSteps = theApp.expNSteps;
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

		for (glm::vec2 mp : globalDeltaSet)
		{
			drawPlaneOrigin(oMan.getShader(1), glm::vec3(mp.x, mp.y, 1.5f), 5.0f, 5.0f, glm::vec3(1.0f, 0.0f, 1.0f), 0, false);
		}

		if (polyPoints.size()>2)
		for (int i = 0; i < polyPoints.size() - 1; i++)
		{
			drawLine(oMan.getShader(1), glm::vec3(polyPoints[i],1.0f), glm::vec3(polyPoints[i+1],1.0f), glm::vec3(0.0f,1.0f,0.0f));
			if (i== polyPoints.size() - 2)
				drawLine(oMan.getShader(1), glm::vec3(polyPoints[i+1], 1.0f), glm::vec3(polyPoints[0], 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (resultingRect.extent.length()>0)
		{
			drawLine(oMan.getShader(1), glm::vec3(resultingRect.center, 1.0f), glm::vec3(resultingRect.center+resultingRect.axis1*resultingRect.extent.x,1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
			drawLine(oMan.getShader(1), glm::vec3(resultingRect.center, 1.0f), glm::vec3(resultingRect.center - resultingRect.axis1*resultingRect.extent.x, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
			drawLine(oMan.getShader(1), glm::vec3(resultingRect.center, 1.0f), glm::vec3(resultingRect.center + resultingRect.axis2*resultingRect.extent.y, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
			drawLine(oMan.getShader(1), glm::vec3(resultingRect.center, 1.0f), glm::vec3(resultingRect.center - resultingRect.axis2*resultingRect.extent.y, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
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