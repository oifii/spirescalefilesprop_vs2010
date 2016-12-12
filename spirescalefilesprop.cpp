/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// spirescalefilesprop.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FreeImage.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;


string global_imagefolder = "C:\\zero-classic";
string global_imageextension = ".jpg";
vector<string> global_imagefilenames;


int _tmain(int argc, _TCHAR* argv[])
{
	//1)
	//////////////////////////
	//populate image filenames
	//////////////////////////
	if (global_imagefolder != "" && global_imageextension != "")
	{
		//1) execute cmd line to get all folder's image filenames
		string quote = "\"";
		string pathfilter;
		string path = global_imagefolder;
		//pathfilter = path + "\\*.bmp";
		pathfilter = path + "\\*" + global_imageextension;
		string systemcommand;
		//systemcommand = "DIR " + quote + pathfilter + quote + "/B /O:N > wsic_filenames.txt"; //wsip tag standing for wav set (library) instrumentset (class) populate (function)
		systemcommand = "DIR " + quote + pathfilter + quote + "/B /S /O:N > spirescalefilesprop_filenames.txt"; // /S for adding path into "spiwtmvs_filenames.txt"
		system(systemcommand.c_str());
		//2) load in all "spiwtmvs_filenames.txt" file
		//vector<string> global_imagefilenames;
		ifstream ifs("spirescalefilesprop_filenames.txt");
		string temp;
		while (getline(ifs, temp))
		{
			//txtfilenames.push_back(path + "\\" + temp);
			global_imagefilenames.push_back(temp);
		}
	}

	/*
	//2)
	//////////////////////////////////////////////////////////////////////
	//for each file, copy the original to another folder renaming the file
	//////////////////////////////////////////////////////////////////////
	string originalfilename="";
	string originalfolder="C:\\zero";
	string newfilename="";
	string newfolder="C:\\zero-classic";

	for (vector<string>::iterator it = global_imagefilenames.begin() ; it != global_imagefilenames.end(); ++it)
	{    
		cout << endl << *it << endl;
		string filename = (*it).substr((*it).find_last_of("\\/")+1);
		cout << filename << endl;
		string subfolder = filename.substr(0,filename.find_first_of("_"));
		cout << subfolder << endl;
		string formerfilename = filename.substr(filename.find_first_of("_")+1);
		originalfilename = originalfolder + "\\" + subfolder + "\\" + formerfilename;
		cout << originalfilename << endl;
		newfilename = newfolder + "\\" + subfolder + "_" + formerfilename;
		cout << newfilename << endl;

		string quote = "\"";
		string systemcommand;
		systemcommand = "COPY " + quote + originalfilename + quote + " " + quote + newfilename + quote; 
		system(systemcommand.c_str());
	}
	*/

	//2)
	//////////////////////////////////////////////////////////////////////
	//for each file, copy the original to another folder renaming the file
	//////////////////////////////////////////////////////////////////////
	string newfilename="";
	string newfolder="C:\\zero-classic(1920x1080-prop)";

	int ii=0;
	for (vector<string>::iterator it = global_imagefilenames.begin() ; it != global_imagefilenames.end(); ++it)
	{    
		ii++;
		//if(ii==5) break;

		cout << endl << *it << endl;
		string filename = (*it).substr((*it).find_last_of("\\/")+1);
		cout << filename << endl;

		FIBITMAP* pDIB = FreeImage_Load(FIF_JPEG, (*it).c_str(), JPEG_DEFAULT);
		if(pDIB)
		{
			//now, decide which factor for a proportional rescaling
			FIBITMAP* pRescaledDIB = NULL;
			FIBITMAP* pOtherDIB = NULL;
			int finalx = 1920;
			int finaly = 1080;
			float ratiox = FreeImage_GetWidth(pDIB)/1920.0f;
			float ratioy = FreeImage_GetHeight(pDIB)/1080.0f;
			if(ratiox>ratioy)
			{
				pRescaledDIB = FreeImage_Rescale(pDIB, 1920, FreeImage_GetHeight(pDIB)/ratiox, FILTER_BICUBIC);
			}
			else
			{
				pRescaledDIB = FreeImage_Rescale(pDIB, FreeImage_GetWidth(pDIB)/ratioy, 1080, FILTER_BICUBIC);
			}

			if(pRescaledDIB)
			{
				//allocate 1080p black image buffer
				pOtherDIB = FreeImage_Allocate(1920, 1080, 24);
				if(pOtherDIB)
				{
					//copy line by line the rescaled image onto 1080p black image buffer
					int ncol = FreeImage_GetWidth(pRescaledDIB);
					int ncol_offset = (1920-ncol)/2;
					int nrow = FreeImage_GetHeight(pRescaledDIB);
					int nrow_offset = (1080-nrow)/2;
					for (int y=0; y<nrow; y++)
					{
						//BYTE* bits = FreeImage_GetScanLine(pRescaledDIB, nrow - y - 1);
						for(int x=0; x<ncol; x++)
						{
							RGBQUAD myRGBQUAD;
							FreeImage_GetPixelColor(pRescaledDIB, x, y, &myRGBQUAD);
							FreeImage_SetPixelColor(pOtherDIB, x+ncol_offset, y+nrow_offset, &myRGBQUAD);
						}
					}

					//save final image buffer
					newfilename = newfolder + "\\" + filename;
					cout << newfilename << endl;
					//FreeImage_Save(FIF_JPEG, pRescaledDIB, newfilename.c_str());
					FreeImage_Save(FIF_JPEG, pOtherDIB, newfilename.c_str());

					FreeImage_Unload(pOtherDIB);
				}
				FreeImage_Unload(pRescaledDIB);
			}
			FreeImage_Unload(pDIB);
		}
	}

	return 0;
}

