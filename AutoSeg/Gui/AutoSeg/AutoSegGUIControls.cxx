/*=========================================================================

  Program:   AutoSeg
  Module:    $RCSfile: AutoSegGUIControls.cxx,v $
  Language:  C++
  Date:      $Date: 2011/02/25 11:19:14 $
  Version:   $Revision: 1.24 $
  Author:    Clement Vachet

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "AutoSegGUIControls.h"
#include <itksys/SystemTools.hxx>

AutoSegGUIControls::AutoSegGUIControls(char *_AutoSegPath)
	: AutoSegGUI()
{
	bool IsDefaultSharpAtlasParameterFile;
	bool IsDefaultFuzzyAtlasParameterFile;  

	char DefaultSharpAtlasParameterFile[512];
	char DefaultFuzzyAtlasParameterFile[512];

	g_MainWindow->show();
	m_BrowserWidths = NULL;
	m_AuxBrowserWidths = NULL;
	
  // Set AutoSeg Environment
	CheckDirectoryName(_AutoSegPath);
	m_Computation.SetAutoSegPath(_AutoSegPath);
  
  // Initialization Computation Parameters
	m_Computation.SetT2Image(g_T2Button->value());
	m_Computation.SetPDImage(g_PDButton->value());
	
	m_Computation.SetAuxT1Image(g_AuxT1Button->value());
	m_Computation.SetAuxT2Image(g_AuxT2Button->value());
	m_Computation.SetAuxPDImage(g_AuxPDButton->value());
	
	m_Computation.SetAux1Image(g_Aux1Button->value());
	m_Computation.SetAux2Image(g_Aux2Button->value());
	m_Computation.SetAux3Image(g_Aux3Button->value());
	m_Computation.SetAux4Image(g_Aux4Button->value());
	m_Computation.SetAux5Image(g_Aux5Button->value());
	m_Computation.SetAux6Image(g_Aux6Button->value());
	m_Computation.SetAux7Image(g_Aux7Button->value());
	m_Computation.SetAux8Image(g_Aux8Button->value());
	
	g_DataAutoSegDirectoryDisp->value("AutoSeg");
	m_Computation.SetDataAutoSegDirectory("AutoSeg");
	m_Computation.SetComputeVolume(g_ComputeVolumeButton->value());
	m_Computation.SetComputeCorticalThickness(g_ComputeCorticalThicknessButton->value());
	m_Computation.SetRecompute(g_RecomputeButton->value());
	m_Computation.SetUseCondor(g_UseCondorButton->value());
  
	m_Computation.SetIsAutoSegInProcess(false);

  // Initialization Default Parameter Files
	std::strcpy(DefaultSharpAtlasParameterFile, m_Computation.GetAutoSegPath());
	std::strcat(DefaultSharpAtlasParameterFile, "AutoSeg_DefaultSharpAtlasParameters.txt");
	SetDefaultSharpAtlasParameterFile(DefaultSharpAtlasParameterFile);

	std::strcpy(DefaultFuzzyAtlasParameterFile, m_Computation.GetAutoSegPath());
	std::strcat(DefaultFuzzyAtlasParameterFile, "AutoSeg_DefaultFuzzyAtlasParameters.txt");
	SetDefaultFuzzyAtlasParameterFile(DefaultFuzzyAtlasParameterFile);
	IsDefaultSharpAtlasParameterFile = LoadParameterFile(GetDefaultSharpAtlasParameterFile());
	if (!IsDefaultSharpAtlasParameterFile)
	{
		IsDefaultFuzzyAtlasParameterFile = LoadParameterFile(GetDefaultFuzzyAtlasParameterFile());
		if (!IsDefaultFuzzyAtlasParameterFile)
			InitializeParameters();
	}
}

AutoSegGUIControls::~AutoSegGUIControls()
{
	if (m_BrowserWidths)
		delete[] m_BrowserWidths;
	
	if (m_AuxBrowserWidths)
		delete[] m_AuxBrowserWidths;
}

void AutoSegGUIControls::AboutButtonPressed()
{
	AboutGUIControls About;
	About.g_MainWindow->show();
	Fl::run();
}

void AutoSegGUIControls::ExitAutoSeg()
{
	if (m_Computation.GetIsAutoSegInProcess())
	{
		if (!fl_choice("Choose one of the following:","Exit without stopping the current process", "Exit and stop the process", "Cancel"))
			g_MainWindow->hide();
		else
		{
			m_Computation.StopBatchMake();
			g_MainWindow->hide();
		}
	}
	else
	{
		if (fl_choice("Do you really want to exit AutoSeg?", "No", "Yes", NULL))
			g_MainWindow->hide();
	}
}

void AutoSegGUIControls::LoadParameterFileGUI()
{
	Fl_File_Chooser fc(".","*.txt",Fl_File_Chooser::SINGLE,"Load a Parameter File");
	fc.show();
	while(fc.shown())
		Fl::wait();

  //if a name has been set
	if(fc.count())
		LoadParameterFile(fc.value());  
}

void AutoSegGUIControls::LoadComputationFileGUI()
{
	Fl_File_Chooser fc(".","*.txt",Fl_File_Chooser::SINGLE,"Load a Computation File");
	fc.show();  
	while(fc.shown())
		Fl::wait();

  //if a name has been set
	if(fc.count())
	{
		g_DataBrowser->clear();
		LoadComputationFile(fc.value());
	}
}

void AutoSegGUIControls::SaveParameterFileGUI()
{
	char *NewFile = NULL;

	NewFile = fl_file_chooser("Save Parameter File as ?","*.txt",".");
	if (NewFile != NULL)
	{
		UpdateParameters();
		m_Computation.WriteParameterFile(NewFile);
	}
}

void AutoSegGUIControls::SaveComputationFileGUI()
{
	char *NewFile = NULL;

	NewFile = fl_file_chooser("Save Computation File as ?","*.txt",".");
	if (NewFile != NULL)
	{
		m_Computation.SetDataAutoSegDirectory(g_DataAutoSegDirectoryDisp->value());
		m_Computation.DesallocateDataList();
		m_Computation.DesallocateAuxDataList();
		InitializeData();
		InitializeAuxData();
		m_Computation.WriteComputationFile(NewFile);
	}
}

void AutoSegGUIControls::UseDefaultSharpAtlasParametersGUI()
{
	LoadParameterFile(GetDefaultSharpAtlasParameterFile());
}

void AutoSegGUIControls::UseDefaultFuzzyAtlasParametersGUI()
{
	LoadParameterFile(GetDefaultFuzzyAtlasParameterFile());
}

void AutoSegGUIControls::SetDefaultSharpAtlasParametersGUI()
{
	if (g_SharpAtlasButton->value())
	{
		if (fl_choice("Do you really want to set the current parameters as default sharp atlas parameters?","No", "Yes", NULL))
		{
			UpdateParameters();
			m_Computation.WriteParameterFile(GetDefaultSharpAtlasParameterFile());      
		}
	}
	else
		fl_alert("'Fuzzy' tissue atlas is selected!\nYou cannot set your current parameters as default sharp atlas parameters...");
}

void AutoSegGUIControls::SetDefaultFuzzyAtlasParametersGUI()
{
	if (g_FuzzyAtlasButton->value())
	{      
		if (fl_choice("Do you really want to set the current parameters as default fuzzy atlas parameters?","No", "Yes", NULL))
		{
			UpdateParameters();
			m_Computation.WriteParameterFile(GetDefaultFuzzyAtlasParameterFile());
		}
	}
	else
		fl_alert("'Sharp' tissue atlas is selected! \nYou cannot set your current parameters as default fuzzy atlas parameters...");
}

void AutoSegGUIControls::ResetDefaultSharpAtlasParametersGUI()
{
	InitializeParameters(0);
	m_Computation.WriteParameterFile(GetDefaultSharpAtlasParameterFile());
}

void AutoSegGUIControls::ResetDefaultFuzzyAtlasParametersGUI()
{
	InitializeParameters(1);
	m_Computation.WriteParameterFile(GetDefaultFuzzyAtlasParameterFile());
}

void AutoSegGUIControls::SetDefaultSharpAtlasParameterFile(const char *_DefaultSharpAtlasParameterFile)
{
	std::strcpy(m_DefaultSharpAtlasParameterFile, _DefaultSharpAtlasParameterFile);  
}

void AutoSegGUIControls::SetDefaultFuzzyAtlasParameterFile(const char *_DefaultFuzzyAtlasParameterFile)
{
	std::strcpy(m_DefaultFuzzyAtlasParameterFile, _DefaultFuzzyAtlasParameterFile);  
}

char * AutoSegGUIControls::GetDefaultSharpAtlasParameterFile()
{
	return m_DefaultSharpAtlasParameterFile;
}

char * AutoSegGUIControls::GetDefaultFuzzyAtlasParameterFile()
{
	return m_DefaultFuzzyAtlasParameterFile;
}

// Load computation file
void AutoSegGUIControls::LoadComputationFile(const char *_FileName)
{
	FILE* ComputationFile;
	char Line[1536];
	char Data[1536];
	int Length;
  // Computation Options
	int IsT2Image, IsPDImage;
	int ComputeVolume, ComputeCorticalThickness, Recompute, UseCondor;  

	if ((ComputationFile = fopen(_FileName,"r")) != NULL) 
	{
		while ( (fgets(Line,1536,ComputationFile)) != NULL)
		{
			Length = std::strlen(Line);
			Line[Length-1] = '\0';      

			if ( (std::strncmp("Process Data Directory: ", Line, 24)) == 0)
			{
				if (std::strlen(Line+24) != 0)
				{
					m_Computation.SetProcessDataDirectory(Line+24); 
					g_ProcessDataDirectoryDisp->value(Line+24);
					g_ProcessDataDirectoryDisp->position(g_ProcessDataDirectoryDisp->size());
				}
				else
				{
					g_ProcessDataDirectoryDisp->value("");  
					m_Computation.SetProcessDataDirectory("");
				}
			}
			else if ( (std::strncmp("Is T2 Image: ", Line, 13)) == 0)
			{
				IsT2Image = atoi(Line+13);
				m_Computation.SetT2Image(IsT2Image); 
				if (IsT2Image == 0)
				{
					g_T2Button->clear();
					g_T2Title->deactivate();
					g_T2Disp->deactivate();
					g_T2Disp->value(NULL);
				}
				else
				{
					g_T2Button->set();	
					g_T2Title->activate();
					g_T2Disp->activate();
				}
			}
			else if ( (std::strncmp("Is PD Image: ", Line, 13)) == 0)
			{
				IsPDImage = atoi(Line+13);
				m_Computation.SetPDImage(IsPDImage); 	 
				if (IsPDImage == 0)
				{
					g_PDButton->clear();
					g_PDTitle->deactivate();
					g_PDDisp->deactivate();
					g_PDDisp->value(NULL);
				}
				else
				{
					g_PDButton->set();
					g_PDTitle->activate();
					g_PDDisp->activate();
				}
				InitBrowser();
			}
			else if( (std::strncmp("Data AutoSeg Directory: ", Line, 24)) == 0)
			{
				if (std::strlen(Line+24))
				{
					m_Computation.SetDataAutoSegDirectory(Line+24);
					g_DataAutoSegDirectoryDisp->value(Line+24);
				}
				else
				{
					m_Computation.SetDataAutoSegDirectory(""); 
					g_DataAutoSegDirectoryDisp->value(NULL);
				}
			}
			else if ( (std::strncmp("Data Directory: ", Line, 16)) == 0)
			{
				if (std::strlen(Line+16) != 0)
				{
					m_Computation.SetDataDirectory(Line+16); 
					g_DataDirectoryDisp->value(Line+16); 
					g_DataDirectoryDisp->position(g_DataDirectoryDisp->size());
				}
				else
				{
					m_Computation.SetDataDirectory(""); 
					g_DataDirectoryDisp->value(NULL);          
				}
			}
			else if ( (std::strncmp("T1 Files: ", Line, 10)) == 0)
			{
				if (std::strlen(Line+10) != 0)
				{
					g_T1Disp->value(Line+10);
					g_T1Disp->position(g_T1Disp->size());
					m_Computation.SetT1(Line+10); 	      
				}
				else
				{
					g_T1Disp->value(NULL);
					m_Computation.SetT1("");
				}
			}
			else if ( (std::strncmp("T2 Files: ", Line, 10)) == 0)
			{
				if (std::strlen(Line+10) != 0)
				{
					g_T2Disp->value(Line+10);	 
					g_T2Disp->position(g_T2Disp->size());
					m_Computation.SetT2(Line+10);
				}
				else
				{
					g_T2Disp->value(NULL);
					m_Computation.SetT2(""); 
				}
			}
			else if ( (std::strncmp("PD Files: ", Line, 10)) == 0)
			{
				if (std::strlen(Line+10) != 0)        
				{
					g_PDDisp->value(Line+10);	  
					g_PDDisp->position(g_PDDisp->size());
					m_Computation.SetPD(Line+10);
				}
				else
				{
					g_PDDisp->value(NULL);	    
					m_Computation.SetPD(""); 
				}     
			}
			else if ( (std::strncmp("Compute Volume: ", Line, 16)) == 0)
			{
				ComputeVolume = atoi(Line+16);
				m_Computation.SetComputeVolume(ComputeVolume);
				if (ComputeVolume == 1)
					g_ComputeVolumeButton->set();
				else
					g_ComputeVolumeButton->clear();
			}
			else if ( (std::strncmp("Compute cortical thickness: ", Line, 28)) == 0)
			{
				ComputeCorticalThickness = atoi(Line+28);
				m_Computation.SetComputeCorticalThickness(ComputeCorticalThickness);
				if (ComputeCorticalThickness == 1)
					g_ComputeCorticalThicknessButton->set();
				else
					g_ComputeCorticalThicknessButton->clear();
			}
			else if ( (std::strncmp("Recompute: ", Line, 11)) == 0)
			{
				Recompute = atoi(Line+11);
				m_Computation.SetRecompute(Recompute);
				if (Recompute == 1)
					g_RecomputeButton->set();
				else
					g_RecomputeButton->clear();
			}
			else if ( (std::strncmp("Use Condor: ", Line, 12)) == 0)
			{
				UseCondor = atoi(Line+12);
				m_Computation.SetUseCondor(UseCondor);
				if (UseCondor == 1)
					g_UseCondorButton->set();
				else
					g_UseCondorButton->clear();
			}
			else if ( (std::strncmp("Data: ", Line, 6)) == 0)
			{
				RightJustifyData(Line+6, Data);
				g_DataBrowser->add(Data);
			}
		}
		fclose(ComputationFile);
	}
	else
		std::cout<<"Error Opening File: "<<_FileName<<std::endl;
}

void AutoSegGUIControls::LoadAuxComputationFile(const char *_FileName)
{
	FILE* AuxComputationFile;
	char Line[1536];
	char Data[1536];
	int Length;
	
  // Computation Options
	int IsAux1Image, IsAux2Image, IsAux3Image, IsAux4Image, IsAux5Image, IsAux6Image, IsAux7Image, IsAux8Image;
	int IsAuxT1Image, IsAuxT2Image, IsAuxPDImage;
	int RigidTransformation, AffineTransformation, BsplineTransformation, AtlasSpaceImage, BiasCorrectedImage, SkullStrippedImage;
	
	if ((AuxComputationFile = fopen(_FileName,"r")) != NULL) 
	{
		while ( (fgets(Line,1536,AuxComputationFile)) != NULL)
		{
			Length = std::strlen(Line);
			Line[Length-1] = '\0';
			if ( (std::strncmp("Is AuxT1 Image: ", Line, 16)) == 0)
			{
				IsAuxT1Image = atoi(Line+16);
				m_Computation.SetAuxT1Image(IsAuxT1Image);
				if (IsAuxT1Image == 1)
				{
					g_AuxT1Button->setonly();
				}
			}
			else if ( (std::strncmp("Is AuxT2 Image: ", Line, 16)) == 0)
			{
				IsAuxT2Image = atoi(Line+16);
				m_Computation.SetAuxT2Image(IsAuxT2Image);
				if (IsAuxT2Image == 1)
				{
					g_AuxT2Button->setonly();
				}
			}
			else
			{
				IsAuxPDImage = atoi(Line+16);
				m_Computation.SetAuxPDImage(IsAuxPDImage);
				if (IsAuxPDImage == 1)
				{
					g_AuxPDButton->setonly();
				}
			}
			if ( (std::strncmp("Is Aux1 Image: ", Line, 15)) == 0)
			{
				IsAux1Image = atoi(Line+15);
				m_Computation.SetAux1Image(IsAux1Image); 	 
				if (IsAux1Image == 0)
				{
					g_Aux1Button->clear();
					g_Aux1LabelDisp->deactivate();
					g_Aux1LabelDisp->value(NULL);
					g_Aux1Disp->deactivate();
					g_Aux1Disp->value(NULL);
					g_Aux1Title->deactivate();
					
					g_Aux2Button->clear();
					g_Aux2Button->deactivate();
					g_Aux2LabelDisp->deactivate();
					g_Aux2LabelDisp->value(NULL);
					g_Aux2Disp->deactivate();
					g_Aux2Disp->value(NULL);
					g_Aux2Title->deactivate();
					
					g_Aux3Button->clear();
					g_Aux3Button->deactivate();
					g_Aux3LabelDisp->deactivate();
					g_Aux3LabelDisp->value(NULL);
					g_Aux3Disp->deactivate();
					g_Aux3Disp->value(NULL);
					g_Aux3Title->deactivate();
					
					g_Aux4Button->clear();
					g_Aux4Button->deactivate();
					g_Aux4LabelDisp->deactivate();
					g_Aux4LabelDisp->value(NULL);
					g_Aux4Disp->deactivate();
					g_Aux4Disp->value(NULL);
					g_Aux4Title->deactivate();
					
					g_Aux5Button->clear();
					g_Aux5Button->deactivate();
					g_Aux5LabelDisp->deactivate();
					g_Aux5LabelDisp->value(NULL);
					g_Aux5Disp->deactivate();
					g_Aux5Disp->value(NULL);
					g_Aux5Title->deactivate();
					
					g_Aux6Button->clear();
					g_Aux6Button->deactivate();
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6Title->deactivate();
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
				}
				else
				{
					g_Aux1Button->set();
					g_Aux1Title->activate();
					g_Aux1Disp->activate();
					g_Aux1LabelDisp->activate();
					g_Aux2Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux2 Image: ", Line, 15)) == 0)
			{
				IsAux2Image = atoi(Line+15);
				m_Computation.SetAux2Image(IsAux2Image); 	 
				if ((IsAux2Image == 0) || ((IsAux2Image == 1) && (IsAux1Image == 0)))
				{
					g_Aux2Button->clear();
					g_Aux2Disp->deactivate();
					g_Aux2Disp->value(NULL);
					g_Aux2LabelDisp->deactivate();
					g_Aux2LabelDisp->value(NULL);
					g_Aux2Title->deactivate();
					
					g_Aux3Button->clear();
					g_Aux3Button->deactivate();
					g_Aux3LabelDisp->deactivate();
					g_Aux3LabelDisp->value(NULL);
					g_Aux3Disp->deactivate();
					g_Aux3Disp->value(NULL);
					g_Aux3Title->deactivate();
					
					g_Aux4Button->clear();
					g_Aux4Button->deactivate();
					g_Aux4LabelDisp->deactivate();
					g_Aux4LabelDisp->value(NULL);
					g_Aux4Disp->deactivate();
					g_Aux4Disp->value(NULL);
					g_Aux4Title->deactivate();
					
					g_Aux5Button->clear();
					g_Aux5Button->deactivate();
					g_Aux5LabelDisp->deactivate();
					g_Aux5LabelDisp->value(NULL);
					g_Aux5Disp->deactivate();
					g_Aux5Disp->value(NULL);
					g_Aux5Title->deactivate();
					
					g_Aux6Button->clear();
					g_Aux6Button->deactivate();
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6Title->deactivate();
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
				}
				else
				{
					g_Aux2Button->set();
					g_Aux2Title->activate();
					g_Aux2Disp->activate();
					g_Aux2LabelDisp->activate();
					g_Aux3Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux3 Image: ", Line, 15)) == 0)
			{
				IsAux3Image = atoi(Line+15);
				m_Computation.SetAux3Image(IsAux3Image); 	 
				if ((IsAux3Image == 0) || ((IsAux3Image == 1) && ((IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux3Button->clear();
					g_Aux3Title->deactivate();
					g_Aux3Disp->deactivate();
					g_Aux3Disp->value(NULL);
					g_Aux3LabelDisp->deactivate();
					g_Aux3LabelDisp->value(NULL);
					
					g_Aux4Button->clear();
					g_Aux4Button->deactivate();
					g_Aux4LabelDisp->deactivate();
					g_Aux4LabelDisp->value(NULL);
					g_Aux4Disp->deactivate();
					g_Aux4Disp->value(NULL);
					g_Aux4Title->deactivate();
					
					g_Aux5Button->clear();
					g_Aux5Button->deactivate();
					g_Aux5LabelDisp->deactivate();
					g_Aux5LabelDisp->value(NULL);
					g_Aux5Disp->deactivate();
					g_Aux5Disp->value(NULL);
					g_Aux5Title->deactivate();
					
					g_Aux6Button->clear();
					g_Aux6Button->deactivate();
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6Title->deactivate();
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
				}
				else
				{
					g_Aux3Button->set();
					g_Aux3Title->activate();
					g_Aux3Disp->activate();
					g_Aux3LabelDisp->activate();
					g_Aux4Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux4 Image: ", Line, 15)) == 0)
			{
				IsAux4Image = atoi(Line+15);
				m_Computation.SetAux4Image(IsAux4Image); 	 
				if ((IsAux4Image == 0) || ((IsAux4Image == 1) && ((IsAux3Image == 0) || (IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux4Button->clear();
					g_Aux4Title->deactivate();
					g_Aux4Disp->deactivate();
					g_Aux4Disp->value(NULL);
					g_Aux4LabelDisp->deactivate();
					g_Aux4LabelDisp->value(NULL);
					
					g_Aux5Button->clear();
					g_Aux5Button->deactivate();
					g_Aux5Disp->deactivate();
					g_Aux5Disp->value(NULL);
					g_Aux5Title->deactivate();
					g_Aux5LabelDisp->deactivate();
					g_Aux5LabelDisp->value(NULL);
					
					g_Aux6Button->clear();
					g_Aux6Button->deactivate();
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6Title->deactivate();
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
				}
				else
				{
					g_Aux4Button->set();
					g_Aux4Title->activate();
					g_Aux4Disp->activate();
					g_Aux4LabelDisp->activate();
					g_Aux5Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux5 Image: ", Line, 15)) == 0)
			{
				IsAux5Image = atoi(Line+15);
				m_Computation.SetAux5Image(IsAux5Image); 	 
				if ((IsAux5Image == 0) || ((IsAux5Image == 1) && ((IsAux4Image == 0) || (IsAux3Image == 0) || (IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux5Button->clear();
					g_Aux5Title->deactivate();
					g_Aux5Disp->deactivate();
					g_Aux5Disp->value(NULL);
					g_Aux5LabelDisp->deactivate();
					g_Aux5LabelDisp->value(NULL);
					
					g_Aux6Button->clear();
					g_Aux6Button->deactivate();
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6Title->deactivate();
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
				}
				else
				{
					g_Aux5Button->set();
					g_Aux5Title->activate();
					g_Aux5Disp->activate();
					g_Aux5LabelDisp->activate();
					g_Aux6Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux6 Image: ", Line, 15)) == 0)
			{
				IsAux6Image = atoi(Line+15);
				m_Computation.SetAux6Image(IsAux6Image); 	 
				if ((IsAux6Image == 0) || ((IsAux6Image == 1) && ((IsAux5Image == 0) || (IsAux4Image == 0) || (IsAux3Image == 0) || (IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux6Button->clear();
					g_Aux6Title->deactivate();
					g_Aux6Disp->deactivate();
					g_Aux6Disp->value(NULL);
					g_Aux6LabelDisp->deactivate();
					g_Aux6LabelDisp->value(NULL);
					
					g_Aux7Button->clear();
					g_Aux7Button->deactivate();
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7Title->deactivate();
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
				}
				else
				{
					g_Aux6Button->set();
					g_Aux6Title->activate();
					g_Aux6Disp->activate();
					g_Aux6LabelDisp->activate();
					g_Aux7Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux7 Image: ", Line, 15)) == 0)
			{
				IsAux7Image = atoi(Line+15);
				m_Computation.SetAux7Image(IsAux7Image); 	 
				if ((IsAux7Image == 0) || ((IsAux7Image == 1) && ((IsAux6Image == 0) || (IsAux5Image == 0) || (IsAux4Image == 0) || (IsAux3Image == 0) || (IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux7Button->clear();
					g_Aux7Title->deactivate();
					g_Aux7Disp->deactivate();
					g_Aux7Disp->value(NULL);
					g_Aux7LabelDisp->deactivate();
					g_Aux7LabelDisp->value(NULL);
					
					g_Aux8Button->clear();
					g_Aux8Button->deactivate();
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8Title->deactivate();
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
				}
				else
				{
					g_Aux7Button->set();
					g_Aux7Title->activate();
					g_Aux7Disp->activate();
					g_Aux7LabelDisp->activate();
					g_Aux8Button->activate();
				}
			}
			else if ( (std::strncmp("Is Aux8 Image: ", Line, 15)) == 0)
			{
				IsAux8Image = atoi(Line+15);
				m_Computation.SetAux8Image(IsAux8Image); 	 
				if ((IsAux8Image == 0) || ((IsAux8Image == 1) && ((IsAux7Image == 0) || (IsAux6Image == 0) || (IsAux5Image == 0) || (IsAux4Image == 0) || (IsAux3Image == 0) || (IsAux2Image == 0) || (IsAux1Image == 0))))
				{
					g_Aux8Button->clear();
					g_Aux8Title->deactivate();
					g_Aux8Disp->deactivate();
					g_Aux8Disp->value(NULL);
					g_Aux8LabelDisp->deactivate();
					g_Aux8LabelDisp->value(NULL);
				}
				else
				{
					g_Aux8Button->set();
					g_Aux8Title->activate();
					g_Aux8Disp->activate();
					g_Aux8LabelDisp->activate();
				}
				InitAuxBrowser();
			}
			else if ( (std::strncmp("Data Directory: ", Line, 16)) == 0)
			{
				if (std::strlen(Line+16) != 0)
				{
					m_Computation.SetDataDirectory(Line+16); 
					g_DataDirectoryDisp->value(Line+16); 
					g_DataDirectoryDisp->position(g_DataDirectoryDisp->size());
				}
				else
				{
					m_Computation.SetDataDirectory("");
					g_DataDirectoryDisp->value(NULL);
				}
			}
			else if ( (std::strncmp("AuxT1 Files: ", Line, 13)) == 0)
			{
				if (std::strlen(Line+13) != 0)
				{
					m_Computation.SetT1(Line+13);
				}
				else
				{
					m_Computation.SetT1("");
				}
			}
			else if ( (std::strncmp("AuxT2 Files: ", Line, 13)) == 0)
			{
				if (std::strlen(Line+13) != 0)
				{
					m_Computation.SetT2(Line+13);
				}
				else
				{
					m_Computation.SetT2("");
				}
			}
			else if ( (std::strncmp("AuxPD Files: ", Line, 13)) == 0)
			{
				if (std::strlen(Line+13) != 0)
				{
					m_Computation.SetPD(Line+13);
				}
				else
				{
					m_Computation.SetPD("");
				}
			}
			else if ( (std::strncmp("Aux1 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux1Disp->value(Line+12);
					g_Aux1Disp->position(g_Aux1Disp->size());
					m_Computation.SetAux1(Line+12);
				}
				else
				{
					g_Aux1Disp->value(NULL);
					m_Computation.SetAux1("");
				}
			}
			else if ( (std::strncmp("Aux2 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux2Disp->value(Line+12);
					g_Aux2Disp->position(g_Aux2Disp->size());
					m_Computation.SetAux2(Line+12);
				}
				else
				{
					g_Aux2Disp->value(NULL);
					m_Computation.SetAux2("");
				}
			}
			else if ( (std::strncmp("Aux3 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux3Disp->value(Line+12);
					g_Aux3Disp->position(g_Aux3Disp->size());
					m_Computation.SetAux3(Line+12);
				}
				else
				{
					g_Aux3Disp->value(NULL);
					m_Computation.SetAux3("");
				}
			}
			else if ( (std::strncmp("Aux4 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux4Disp->value(Line+12);
					g_Aux4Disp->position(g_Aux4Disp->size());
					m_Computation.SetAux4(Line+12);
				}
				else
				{
					g_Aux4Disp->value(NULL);
					m_Computation.SetAux4("");
				}
			}
			else if ( (std::strncmp("Aux5 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux5Disp->value(Line+12);
					g_Aux5Disp->position(g_Aux5Disp->size());
					m_Computation.SetAux5(Line+12);
				}
				else
				{
					g_Aux5Disp->value(NULL);
					m_Computation.SetAux5("");
				}
			}
			else if ( (std::strncmp("Aux6 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux6Disp->value(Line+12);
					g_Aux6Disp->position(g_Aux6Disp->size());
					m_Computation.SetAux6(Line+12);
				}
				else
				{
					g_Aux6Disp->value(NULL);
					m_Computation.SetAux6("");
				}
			}
			else if ( (std::strncmp("Aux7 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux7Disp->value(Line+12);
					g_Aux7Disp->position(g_Aux7Disp->size());
					m_Computation.SetAux7(Line+12);
				}
				else
				{
					g_Aux7Disp->value(NULL);
					m_Computation.SetAux7("");
				}
			}
			else if ( (std::strncmp("Aux8 Files: ", Line, 12)) == 0)
			{
				if (std::strlen(Line+12) != 0)
				{
					g_Aux8Disp->value(Line+12);
					g_Aux8Disp->position(g_Aux8Disp->size());
					m_Computation.SetAux8(Line+12);
				}
				else
				{
					g_Aux8Disp->value(NULL);
					m_Computation.SetAux8("");
				}
			}
			else if ( (std::strncmp("Atlas Space Image: ", Line, 19)) == 0)
			{
				AtlasSpaceImage = atoi(Line+19);
				m_Computation.SetAtlasSpaceImage(AtlasSpaceImage);
				if (AtlasSpaceImage == 1)
				{
					g_AtlasSpaceButton->set();
					g_StrippedButton->clear();
					g_BiasCorrectedButton->clear();
					BiasCorrectedImage = 0;
					SkullStrippedImage = 0;
				}
				else
					g_AtlasSpaceButton->clear();
			}
			else if ( (std::strncmp("Bias Corrected Image: ", Line, 22)) == 0)
			{
				BiasCorrectedImage = atoi(Line+22);
				m_Computation.SetBiasCorrectedImage(BiasCorrectedImage);
				if (BiasCorrectedImage == 1)
				{
					g_BiasCorrectedButton->set();
					g_AtlasSpaceButton->clear();
					g_StrippedButton->clear();
					AtlasSpaceImage = 0;
					SkullStrippedImage = 0;
				}
				else
					g_BiasCorrectedButton->clear();
			}
			else if ( (std::strncmp("Skull Stripped Image: ", Line, 22)) == 0)
			{
				SkullStrippedImage = atoi(Line+22);
				m_Computation.SetSkullStrippedImage(SkullStrippedImage);
				if (SkullStrippedImage == 1)
				{
					g_StrippedButton->set();
					g_BiasCorrectedButton->clear();
					g_AtlasSpaceButton->clear();
					AtlasSpaceImage = 0;
					BiasCorrectedImage = 0;
				}
				else
					g_StrippedButton->clear();
			}
			else if ( (std::strncmp("Rigid Transformation: ", Line, 22)) == 0)
			{
				RigidTransformation = atoi(Line+22);
				m_Computation.SetRigidTransformation(RigidTransformation);
				if (RigidTransformation == 1)
				{
					g_RigidTransformationButton->set();
					g_AffineTransformationButton->clear();
					g_BsplineTransformationButton->clear();
					AffineTransformation = 0;
					BsplineTransformation = 0;
				}
				else
					g_RigidTransformationButton->clear();
			}
			else if ( (std::strncmp("Affine Transformation: ", Line, 23)) == 0)
			{
				AffineTransformation = atoi(Line+23);
				m_Computation.SetAffineTransformation(AffineTransformation);
				if (AffineTransformation == 1)
				{
					g_AffineTransformationButton->set();
					g_RigidTransformationButton->clear();
					g_BsplineTransformationButton->clear();
					BsplineTransformation = 0;
					RigidTransformation = 0;
				}
				else
					g_AffineTransformationButton->clear();
			}
			else if ( (std::strncmp("Bspline Transformation: ", Line, 24)) == 0)
			{
				BsplineTransformation = atoi(Line+24);
				m_Computation.SetBsplineTransformation(BsplineTransformation);
				if (BsplineTransformation == 1)
				{
					g_BsplineTransformationButton->set();
					g_AffineTransformationButton->clear();
					g_RigidTransformationButton->clear();
					AffineTransformation = 0;
					RigidTransformation = 0;
				}
				else
					g_BsplineTransformationButton->clear();
			}
			else if ( (std::strncmp("AuxData: ", Line, 9)) == 0)
			{
				RightJustifyAuxData(Line+9, Data);
				g_AuxDataBrowser->add(Data);	
				
			}
		}
		fclose(AuxComputationFile);
	}
	else
	{
		std::cout<<"Error Opening File: "<<_FileName<<std::endl;
	}
}


// Load parameter file
// Mode = file: Read the total file
// Mode = advancedParameters: Read only the advanced parameters (tissue segmentation, warping parameters and N4 ITK Bias Field Correction parameters)
// Mode = tissueSeg: Read only the tissue segmentation parameters
// Mode = warping: Read only the warping parameters
// Mode = N4biasFieldCorrection: Read only the N4 ITK Bias Field Correction parameters
bool AutoSegGUIControls::LoadParameterFile(const char *_FileName, enum Mode mode)
{
	FILE* ParameterFile;
	char Line[512]; 
	int Length;
    // Tissue Segmentation
	int FilterIterations, MaxBiasDegree, Loop;
	float FilterTimeStep, Prior1, Prior2, Prior3, Prior4, FluidAtlasWarpMaxStep;
	int FluidAtlasWarp, FluidAtlasFATW, FluidAtlasAffine, FluidAtlasWarpIterations, LoopIteration;
    // Rigid Registration
	int RigidRegistration, IsROIAtlasGridTemplate;
	int GridTemplateSizeX, GridTemplateSizeY, GridTemplateSizeZ;
	float GridTemplateSpacingX, GridTemplateSpacingY, GridTemplateSpacingZ;
    // Atlas Warping
	float Alpha, Beta, Gamma, MaxPerturbation, NumBasis,DeformationFieldSmoothingSigma;
	int Scale4NbIterations, Scale2NbIterations, Scale1NbIterations,PyramidLevels;
	std::string RegistrationFilterType,MovingShrinkFactors,FixedShrinkFactors,IterationCountPyramidLevels;
    // Skull Stripping
	int DeleteVessels;
    // Regional histogram
	float PointSpacing;
    // N4 ITK Bias Field Correction
	std::string NbOfIterations,BSplineGridResolutions,HistogramSharpening;
	int N4ITKBiasFieldCorrection,ShrinkFactor,BSplineOrder;
	float ConvergenceThreshold,BSplineBeta,BSplineAlpha,SplineDistance;
  
	bool IsParameterFileLoaded=false;

	if (mode ==file)
	{
		// INIT / default setting for backward comaptibility
		
		// Init for EMS loop
		g_LoopButton->clear();
		g_LoopIteration->deactivate();
		g_AtlasLoopGroup->deactivate();
		m_Computation.SetLoop(0);
		// Init for N4
		g_N4ITKBiasFieldCorrectionButton->clear();
		g_N4ParametersGroup->deactivate();
		g_N4AdvancedParametersGroup->deactivate();
		m_Computation.SetN4ITKBiasFieldCorrection(0);	
	}

	if ((ParameterFile = fopen(_FileName,"r")) != NULL) 
	{
		IsParameterFileLoaded = true;
		while ( (fgets(Line,512,ParameterFile)) != NULL)
		{
			Length = std::strlen(Line);
			Line[Length-1] = '\0';
      
			if (mode == file)
			{
				if ( (std::strncmp("Tissue Atlas: ", Line, 14)) == 0)
				{	
					m_Computation.SetTissueAtlas(Line+14);
					if (std::strcmp(Line+14, "Sharp") == 0)
					{
						g_SharpAtlasButton->set();
						g_FuzzyAtlasButton->clear();
					}
					else if (std::strcmp(Line+14, "Fuzzy") == 0)
					{
						g_SharpAtlasButton->clear();
						g_FuzzyAtlasButton->set();
					}
				}	  
	  // Backward compatibility AutoSeg < 1.2
				if ( (std::strncmp("Age Group: ", Line, 11)) == 0)
				{
					if (std::strcmp(Line+11, "Adult") == 0)
					{	    
						g_SharpAtlasButton->clear();
						g_FuzzyAtlasButton->set();
						m_Computation.SetTissueAtlas("Fuzzy");	      
					}
					else if (std::strcmp(Line+11, "Elderly") == 0)
					{
						g_SharpAtlasButton->set();
						g_FuzzyAtlasButton->clear();
						m_Computation.SetTissueAtlas("Sharp");	      
					}
				}
				else if ( (std::strncmp("Common Coordinate Image: ", Line, 25)) == 0)
				{
					if (std::strlen(Line+25) != 0)
					{
						m_Computation.SetCommonCoordinateImage(Line+25); 
						g_CommonCoordinateImageDisp->value(Line+25);
						g_CommonCoordinateImageDisp->position(g_CommonCoordinateImageDisp->size());
					}
					else
					{
						m_Computation.SetCommonCoordinateImage("");
						g_CommonCoordinateImageDisp->value(NULL);
					}
				}
				else if ( (std::strncmp("Common Coordinate Image Type: ", Line, 30)) == 0)
				{
					m_Computation.SetCommonCoordinateImageType(Line+30);
					if (std::strcmp(Line+30, "T1") == 0)
					{
						g_CommonCoordinateImageT1Button->set();		
						g_CommonCoordinateImageT2Button->clear();
					}
					else
					{
						g_CommonCoordinateImageT1Button->clear();		
						g_CommonCoordinateImageT2Button->set();
					}
				}
				else if ( (std::strncmp("Tissue Segmentation Atlas Directory: ", Line, 37)) == 0)
				{
					if (std::strlen(Line+37) != 0)
					{
						m_Computation.SetTissueSegmentationAtlasDirectory(Line+37); 
						g_TissueSegmentationAtlasDirectoryDisp->value(Line+37); 
						g_TissueSegmentationAtlasDirectoryDisp->position(g_TissueSegmentationAtlasDirectoryDisp->size());
					}
					else
					{
						m_Computation.SetTissueSegmentationAtlasDirectory("");
						g_TissueSegmentationAtlasDirectoryDisp->value(NULL);
					}
				}
				else if ( (std::strncmp("Tissue Segmentation Atlas Type: ", Line, 32)) == 0)
				{
					m_Computation.SetTissueSegmentationAtlasType(Line+32);
					if (std::strcmp(Line+32, "T1") == 0)
					{
						g_TissueSegmentationAtlasT1Button->set();		
						g_TissueSegmentationAtlasT2Button->clear();
					}
					else
					{
						g_TissueSegmentationAtlasT1Button->clear();		
						g_TissueSegmentationAtlasT2Button->set();
					}	
				}
				else if ( (std::strncmp("ROI Atlas File: ", Line, 16)) == 0)
				{
					if (std::strlen(Line+16) != 0)
					{
						m_Computation.SetROIAtlasFile(Line+16); 
						g_ROIAtlasFileDisp->value(Line+16);
						g_ROIAtlasFileDisp->position(g_ROIAtlasFileDisp->size());
					}
					else
					{
						m_Computation.SetROIAtlasFile("");
						g_ROIAtlasFileDisp->value(NULL);
					}
				}
				else if ( (std::strncmp("Amygdala Left: ", Line, 15)) == 0)
				{
					if (std::strlen(Line+15) != 0)
					{
						g_AmygdalaLeftButton->set();
						m_Computation.SetAmygdalaLeft(Line+15); 
						g_AmygdalaLeftDisp->activate();
						g_AmygdalaLeftDisp->value(Line+15); 
						g_AmygdalaLeftDisp->position(g_AmygdalaLeftDisp->size());
					}
					else
					{
						g_AmygdalaLeftButton->clear();
						g_AmygdalaLeftDisp->deactivate();
						g_AmygdalaLeftDisp->value(NULL);
					}
				}         
				else if ( (std::strncmp("Amygdala Right: ", Line, 16)) == 0)
				{
					if (std::strlen(Line+16) != 0)
					{
						g_AmygdalaRightButton->set();
						m_Computation.SetAmygdalaRight(Line+16);
						g_AmygdalaRightDisp->activate();
						g_AmygdalaRightDisp->value(Line+16);   
						g_AmygdalaRightDisp->position(g_AmygdalaRightDisp->size());
					}
					else
					{
						g_AmygdalaRightButton->clear();
						g_AmygdalaRightDisp->deactivate();
						g_AmygdalaRightDisp->value(NULL);
					}
				}     
				else if ( (std::strncmp("Caudate Left: ", Line, 14)) == 0)
				{
					if (std::strlen(Line+14) != 0)
					{
						g_CaudateLeftButton->set();
						m_Computation.SetCaudateLeft(Line+14); 
						g_CaudateLeftDisp->activate();
						g_CaudateLeftDisp->value(Line+14); 
						g_CaudateLeftDisp->position(g_CaudateLeftDisp->size());
					}
					else
					{
						g_CaudateLeftButton->clear();
						g_CaudateLeftDisp->deactivate();
						g_CaudateLeftDisp->value(NULL);  
					}
				}     
				else if ( (std::strncmp("Caudate Right: ", Line, 15)) == 0)
				{
					if (std::strlen(Line+15) != 0)
					{
						g_CaudateRightButton->set();
						m_Computation.SetCaudateRight(Line+15); 
						g_CaudateRightDisp->activate();
						g_CaudateRightDisp->value(Line+15); 
						g_CaudateRightDisp->position(g_CaudateRightDisp->size());
					}
					else
					{
						g_CaudateRightButton->clear();
						g_CaudateRightDisp->deactivate();
						g_CaudateRightDisp->value(NULL);
					}
				}     
				else if ( (std::strncmp("Hippocampus Left: ", Line, 18)) == 0)
				{
					if (std::strlen(Line+18) != 0)
					{
						g_HippocampusLeftButton->set();
						m_Computation.SetHippocampusLeft(Line+18); 
						g_HippocampusLeftDisp->activate();
						g_HippocampusLeftDisp->value(Line+18); 
						g_HippocampusLeftDisp->position(g_HippocampusLeftDisp->size());
					}
					else
					{
						g_HippocampusLeftButton->clear();
						g_HippocampusLeftDisp->deactivate();
						g_HippocampusLeftDisp->value(NULL);
					}
				}     
				else if ( (std::strncmp("Hippocampus Right: ", Line, 19)) == 0)
				{
					if (std::strlen(Line+19) != 0)
					{
						g_HippocampusRightButton->set();
						m_Computation.SetHippocampusRight(Line+19); 
						g_HippocampusRightDisp->activate();
						g_HippocampusRightDisp->value(Line+19); 
						g_HippocampusRightDisp->position(g_HippocampusRightDisp->size());
					}
					else
					{
						g_HippocampusRightButton->clear();
						g_HippocampusRightDisp->deactivate();
						g_HippocampusRightDisp->value(NULL); 
					}
				}     
				else if ( (std::strncmp("Pallidus Left: ", Line, 15)) == 0)
				{
					if (std::strlen(Line+15) != 0)
					{
						g_PallidusLeftButton->set();
						m_Computation.SetPallidusLeft(Line+15); 
						g_PallidusLeftDisp->activate();
						g_PallidusLeftDisp->value(Line+15);
						g_PallidusLeftDisp->position(g_PallidusLeftDisp->size());
					}
					else
					{
						g_PallidusLeftButton->clear();
						g_PallidusLeftDisp->deactivate();
						g_PallidusLeftDisp->value(NULL);
					}
				}     
				else if ( (std::strncmp("Pallidus Right: ", Line, 16)) == 0)
				{
					if (std::strlen(Line+16) != 0)
					{
						g_PallidusRightButton->set();
						m_Computation.SetPallidusRight(Line+16); 
						g_PallidusRightDisp->activate();
						g_PallidusRightDisp->value(Line+16);
						g_PallidusRightDisp->position(g_PallidusRightDisp->size());
					}
					else
					{
						g_PallidusRightButton->clear();
						g_PallidusRightDisp->deactivate();
						g_PallidusRightDisp->value(NULL); 
					}
				}     
				else if ( (std::strncmp("Putamen Left: ", Line, 14)) == 0)
				{
					if (std::strlen(Line+14) != 0)
					{
						g_PutamenLeftButton->set();
						m_Computation.SetPutamenLeft(Line+14); 
						g_PutamenLeftDisp->activate();
						g_PutamenLeftDisp->value(Line+14); 
						g_PutamenLeftDisp->position(g_PutamenLeftDisp->size());
					}
					else
					{
						g_PutamenLeftButton->clear();
						g_PutamenLeftDisp->deactivate();
						g_PutamenLeftDisp->value(NULL);  
					}
				}     
				else if ( (std::strncmp("Putamen Right: ", Line, 15)) == 0)
				{
					if (std::strlen(Line+15) != 0)
					{
						g_PutamenRightButton->set();
						m_Computation.SetPutamenRight(Line+15); 
						g_PutamenRightDisp->activate();
						g_PutamenRightDisp->value(Line+15); 
						g_PutamenRightDisp->position(g_PutamenRightDisp->size());
					}
					else
					{
						g_PutamenRightButton->clear();
						g_PutamenRightDisp->deactivate();
						g_PutamenRightDisp->value(NULL); 
					}
				}     
				else if ( (std::strncmp("Lateral Ventricle Left: ", Line, 24)) == 0)
				{
					if (std::strlen(Line+24) != 0)
					{
						g_LateralVentricleLeftButton->set();
						m_Computation.SetLateralVentricleLeft(Line+24); 
						g_LateralVentricleLeftDisp->activate();
						g_LateralVentricleLeftDisp->value(Line+24); 
						g_LateralVentricleLeftDisp->position(g_LateralVentricleLeftDisp->size());
					}   
					else
					{
						g_LateralVentricleLeftButton->clear();
						g_LateralVentricleLeftDisp->deactivate();
						g_LateralVentricleLeftDisp->value(NULL); 
					}
				}
				else if ( (std::strncmp("Lateral Ventricle Right: ", Line, 25)) == 0)
				{
					if (std::strlen(Line+25) != 0)
					{
						g_LateralVentricleRightButton->set();
						m_Computation.SetLateralVentricleRight(Line+25); 
						g_LateralVentricleRightDisp->activate();
						g_LateralVentricleRightDisp->value(Line+25); 
						g_LateralVentricleRightDisp->position(g_LateralVentricleRightDisp->size());
					}
					else
					{
						g_LateralVentricleRightButton->clear();
						g_LateralVentricleRightDisp->deactivate();
						g_LateralVentricleRightDisp->value(NULL); 
					}
				}
				else if ( (std::strncmp("ROI File 1: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						g_ROIFile1Button->set();
						m_Computation.SetROIFile1(Line+12); 
						g_ROIFile1Disp->activate();
						g_ROIFile1Disp->value(Line+12); 
						g_ROIFile1Disp->position(g_ROIFile1Disp->size());
					}
					else
					{
						g_ROIFile1Button->clear();
						g_ROIFile1Disp->deactivate();
						g_ROIFile1Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("ROI File 2: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						g_ROIFile2Button->set();
						m_Computation.SetROIFile2(Line+12); 
						g_ROIFile2Disp->activate();
						g_ROIFile2Disp->value(Line+12); 
						g_ROIFile2Disp->position(g_ROIFile2Disp->size());
					}
					else
					{
						g_ROIFile2Button->clear();
						g_ROIFile2Disp->deactivate();
						g_ROIFile2Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("ROI File 3: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						g_ROIFile3Button->set();
						m_Computation.SetROIFile3(Line+12); 
						g_ROIFile3Disp->activate();
						g_ROIFile3Disp->value(Line+12); 
						g_ROIFile3Disp->position(g_ROIFile3Disp->size());
					}
					else
					{
						g_ROIFile3Button->clear();
						g_ROIFile3Disp->deactivate();
						g_ROIFile3Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("ROI File 4: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						g_ROIFile4Button->set();
						m_Computation.SetROIFile4(Line+12); 
						g_ROIFile4Disp->activate();
						g_ROIFile4Disp->value(Line+12); 
						g_ROIFile4Disp->position(g_ROIFile4Disp->size());
					}
					else
					{
						g_ROIFile4Button->clear();
						g_ROIFile4Disp->deactivate();
						g_ROIFile4Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("ROI File 5: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						g_ROIFile5Button->set();
						m_Computation.SetROIFile5(Line+12); 
						g_ROIFile5Disp->activate();
						g_ROIFile5Disp->value(Line+12); 
						g_ROIFile5Disp->position(g_ROIFile5Disp->size());
					}
					else
					{
						g_ROIFile5Button->clear();
						g_ROIFile5Disp->deactivate();
						g_ROIFile5Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("Parcellation File 1: ", Line, 21)) == 0)
				{
					if (std::strlen(Line+21) != 0)
					{
						g_ParcellationFile1Button->set();
						m_Computation.SetParcellationFile1(Line+21); 
						g_ParcellationFile1Disp->activate();
						g_ParcellationFile1Disp->value(Line+21); 
						g_ParcellationFile1Disp->position(g_ParcellationFile1Disp->size());
					}
					else
					{
						g_ParcellationFile1Button->clear();
						g_ParcellationFile1Disp->deactivate();
						g_ParcellationFile1Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("Parcellation File 2: ", Line, 21)) == 0)
				{
					if (std::strlen(Line+21) != 0)
					{
						g_ParcellationFile2Button->set();
						m_Computation.SetParcellationFile2(Line+21); 
						g_ParcellationFile2Disp->activate();
						g_ParcellationFile2Disp->value(Line+21); 
						g_ParcellationFile2Disp->position(g_ParcellationFile2Disp->size());
					}
					else
					{
						g_ParcellationFile2Button->clear();
						g_ParcellationFile2Disp->deactivate();
						g_ParcellationFile2Disp->value(NULL); 
					}
				}
				else if ( (std::strncmp("Parcellation File 3: ", Line, 21)) == 0)
				{
					if (std::strlen(Line+21) != 0)
					{
						g_ParcellationFile3Button->set();
						m_Computation.SetParcellationFile3(Line+21); 
						g_ParcellationFile3Disp->activate();
						g_ParcellationFile3Disp->value(Line+21); 
						g_ParcellationFile3Disp->position(g_ParcellationFile3Disp->size());
					}
					else
					{
						g_ParcellationFile3Button->clear();
						g_ParcellationFile3Disp->deactivate();
						g_ParcellationFile3Disp->value(NULL); 
					}
				}
				else if ((std::strncmp("Rigid Registration: ", Line, 20)) == 0)
				{
					RigidRegistration = (atoi(Line+20));
					m_Computation.SetRigidRegistration(RigidRegistration);
					if (RigidRegistration == 1)
					{
						g_RigidRegistrationButton->set();
						g_RigidRegistrationGroup->activate();
					}
					else
					{
						g_RigidRegistrationButton->clear();
						g_RigidRegistrationGroup->deactivate();
					}
				}
				else if ((std::strncmp("Is ROIAtlasGridTemplate: ", Line, 25)) == 0)
				{
					IsROIAtlasGridTemplate = atoi(Line+25);
					m_Computation.SetROIAtlasGridTemplate(IsROIAtlasGridTemplate);
					if (IsROIAtlasGridTemplate)
					{
						g_GridTemplateAtlasButton->set();
						g_GridTemplateManualButton->clear();
						g_GridParametersGroup->deactivate();
					}
					else
					{
						g_GridTemplateAtlasButton->clear();
						g_GridTemplateManualButton->set();
						g_GridParametersGroup->activate();
					}
				}
				else if ((std::strncmp("GridTemplate SizeX: ", Line, 20)) == 0)
				{
					GridTemplateSizeX = atoi(Line+20);
					m_Computation.SetGridTemplateSizeX(GridTemplateSizeX);
					g_GridTemplateSizeX->value(GridTemplateSizeX);
				}
				else if ((std::strncmp("GridTemplate SizeY: ", Line, 20)) == 0)
				{
					GridTemplateSizeY = atoi(Line+20);
					m_Computation.SetGridTemplateSizeY(GridTemplateSizeY);
					g_GridTemplateSizeY->value(GridTemplateSizeY);
				}
				else if ((std::strncmp("GridTemplate SizeZ: ", Line, 20)) == 0)
				{
					GridTemplateSizeZ = atoi(Line+20);
					m_Computation.SetGridTemplateSizeZ(GridTemplateSizeZ);
					g_GridTemplateSizeZ->value(GridTemplateSizeZ);
				}
				else if ((std::strncmp("GridTemplate SpacingX: ", Line, 23)) == 0)
				{
					GridTemplateSpacingX = atof(Line+23);
					m_Computation.SetGridTemplateSpacingX(GridTemplateSpacingX);
					g_GridTemplateSpacingX->value(GridTemplateSpacingX);
				}
				else if ((std::strncmp("GridTemplate SpacingY: ", Line, 23)) == 0)
				{
					GridTemplateSpacingY = atof(Line+23);
					m_Computation.SetGridTemplateSpacingY(GridTemplateSpacingY);
					g_GridTemplateSpacingY->value(GridTemplateSpacingY);
				}
				else if ((std::strncmp("GridTemplate SpacingZ: ", Line, 23)) == 0)
				{
					GridTemplateSpacingZ = atof(Line+23);
					m_Computation.SetGridTemplateSpacingZ(GridTemplateSpacingZ);
					g_GridTemplateSpacingZ->value(GridTemplateSpacingZ);
				}
				else if ((std::strncmp("Delete Vessels: ", Line, 16)) == 0)
				{
					DeleteVessels = (atoi(Line+16));
					m_Computation.SetDeleteVessels(DeleteVessels);
					if (DeleteVessels == 1)
						g_DeleteVesselsButton->set();
					else
						g_DeleteVesselsButton->clear();
				}
				else if ((std::strncmp("Intensity Rescaling: ", Line, 21)) == 0)
				{
					if (std::strcmp(Line+21,"Histogram quantile") == 0)
					{
						m_Computation.SetIntensityRescalingMethod(1);
						g_HistogramQuantileButton->set();
						g_TissueMeanMatchButton->clear();
					}
					else
					{
						m_Computation.SetIntensityRescalingMethod(2);
						g_TissueMeanMatchButton->set();
						g_HistogramQuantileButton->clear();
					}
				}
				else if ( (std::strncmp("Quantiles: ", Line, 11)) == 0)
				{
					m_Computation.SetQuantiles(Line+11);
					g_QuantilesDisp->value(Line+11);		
				}
				else if ( (std::strncmp("Point Spacing: ", Line, 15)) == 0)
				{
					PointSpacing = atof(Line+15);
					m_Computation.SetPointSpacing(PointSpacing);
					g_PointSpacingDisp->value(PointSpacing);
				}
			}
			if (mode == tissueSeg ||mode == advancedParameters||mode == file)
			{
				if ((std::strncmp("EM Software: ", Line, 13)) == 0)
				{	
					m_Computation.SetEMSoftware(Line+13);
					if (std::strcmp(Line+13,"ABC") == 0)
					{
						g_ABCButton->set();
						g_FilterMethodChoice->activate();
						g_FluidAtlasWarpGroup->activate();
					}
					else
						std::cout<<"No such EM Software!"<<std::endl;	      
				}
				else if ( (std::strncmp("Filter Iterations: ", Line, 19)) == 0)
				{
					FilterIterations = atoi(Line+19);
					m_Computation.SetFilterIterations(FilterIterations);
					g_FilterIterations->value(FilterIterations);	
				}
				else if ( (std::strncmp("Filter TimeStep: ", Line, 17)) == 0)
				{
					FilterTimeStep = atof(Line+17);
					m_Computation.SetFilterTimeStep(FilterTimeStep);
					g_FilterTimeStep->value(FilterTimeStep);	
				}
				else if ( (std::strncmp("Filter Method: ", Line, 15)) == 0)
				{
					m_Computation.SetFilterMethod(Line+15);
					if (std::strcmp(Line+15, "Curvature flow") == 0)
						g_FilterMethodChoice->value(1);
					else
						g_FilterMethodChoice->value(0);
				}
				else if ( (std::strncmp("Max Bias Degree: ", Line, 17)) == 0)
				{
					MaxBiasDegree = atoi(Line+17);
					m_Computation.SetMaxBiasDegree(MaxBiasDegree);
					g_MaxBiasDegree->value(MaxBiasDegree);	
				}
				else if ( (std::strncmp("Prior 1: ", Line, 9)) == 0)
				{
					Prior1 = atof(Line+9);
					m_Computation.SetPrior1(Prior1);
					g_Prior1->value(Prior1);	
				}
				else if ( (std::strncmp("Prior 2: ", Line, 9)) == 0)
				{
					Prior2 = atof(Line+9);
					m_Computation.SetPrior2(Prior2);
					g_Prior2->value(Prior2);	
				}
				else if ( (std::strncmp("Prior 3: ", Line, 9)) == 0)
				{
					Prior3 = atof(Line+9);
					m_Computation.SetPrior3(Prior3);
					g_Prior3->value(Prior3);	
				}
				else if ( (std::strncmp("Prior 4: ", Line, 9)) == 0)
				{
					Prior4 = atof(Line+9);
					m_Computation.SetPrior4(Prior4);
					g_Prior4->value(Prior4);	
				}
				else if ( (std::strncmp("Fluid Atlas Warp: ", Line, 18)) == 0)
				{
					FluidAtlasWarp = atoi(Line+18);
					if (FluidAtlasWarp == 1)
					{
						g_FluidAtlasWarpButton->set();
						g_FluidAtlasAffineButton->clear();
						g_FluidAtlasFATWButton->clear();
						m_Computation.SetFluidAtlasWarp(1);
						m_Computation.SetFluidAtlasAffine(0);
						m_Computation.SetFluidAtlasFATW(0);
					}
					else
					{
						g_FluidAtlasWarpButton->clear();
						m_Computation.SetFluidAtlasWarp(0);
					}	    
				}
				else if ( (std::strncmp("Fluid Atlas Affine: ", Line, 20)) == 0)
				{
					FluidAtlasAffine = atoi(Line+20);
					if (FluidAtlasAffine == 1)
					{
						g_FluidAtlasAffineButton->set();
						g_FluidAtlasWarpButton->clear();
						g_FluidAtlasFATWButton->clear();
						m_Computation.SetFluidAtlasAffine(1);
						m_Computation.SetFluidAtlasWarp(0);
						m_Computation.SetFluidAtlasFATW(0);
					}
					else
					{
						g_FluidAtlasAffineButton->clear();
						m_Computation.SetFluidAtlasAffine(0);
					}	    
				}
				else if ( (std::strncmp("Fluid Atlas FATW: ", Line, 18)) == 0)
				{
					FluidAtlasFATW = atoi(Line+18);
					if (FluidAtlasFATW == 1)
					{
						g_FluidAtlasFATWButton->set();
						g_FluidAtlasWarpButton->clear();
						g_FluidAtlasAffineButton->clear();
						m_Computation.SetFluidAtlasFATW(1);
						m_Computation.SetFluidAtlasWarp(0);
						m_Computation.SetFluidAtlasAffine(0);
					}
					else
					{
						g_FluidAtlasFATWButton->clear();
						m_Computation.SetFluidAtlasFATW(0);
					}	    
				}
				else if ( (std::strncmp("Fluid Atlas Warp Iterations: ", Line, 29)) == 0)
				{
					FluidAtlasWarpIterations = atoi(Line+29);
					m_Computation.SetFluidAtlasWarpIterations(FluidAtlasWarpIterations);
					g_FluidAtlasWarpIterations->value(FluidAtlasWarpIterations);	
				}
				else if ( (std::strncmp("Fluid Atlas Warp Max Step: ", Line, 27)) == 0)
				{
					FluidAtlasWarpMaxStep = atof(Line+27);
					m_Computation.SetFluidAtlasWarpMaxStep(FluidAtlasWarpMaxStep);
					g_FluidAtlasWarpMaxStep->value(FluidAtlasWarpMaxStep);	
				}
				else if ( (std::strncmp("Atlas Linear Mapping: ", Line, 22)) == 0)
				{
					m_Computation.SetAtlasLinearMapping(Line+22);
					if (std::strcmp(Line+22, "affine") == 0)
						g_AtlasLinearMappingChoice->value(0);
					else if (std::strcmp(Line+22, "affine") == 0)
						g_AtlasLinearMappingChoice->value(1);
					else
						g_AtlasLinearMappingChoice->value(2);
				}
				else if ( (std::strncmp("Image Linear Mapping: ", Line, 22)) == 0)
				{
					m_Computation.SetImageLinearMapping(Line+22);
					if (std::strcmp(Line+22, "id") == 0)
						g_ImageLinearMappingChoice->value(0);
					else if (std::strcmp(Line+22, "rigid") == 0)
						g_ImageLinearMappingChoice->value(1);
					else
						g_ImageLinearMappingChoice->value(2);
				}
				else if ((std::strncmp("Loop: ", Line, 6)) == 0)
				{
					Loop= atoi(Line+6);
					if (mode==tissueSeg)
					{
						g_LoopButton->set();
						g_AtlasLoopGroup->activate();
						g_LoopIteration->activate();
						m_Computation.SetLoop(1);
					}
					else
					{
						if (Loop == 1)
						{
							g_LoopButton->set();
							g_AtlasLoopGroup->activate();
							g_LoopIteration->activate();
							m_Computation.SetLoop(1);
						}
						else
						{
							g_LoopButton->clear();
							g_AtlasLoopGroup->deactivate();
							g_LoopIteration->deactivate();
							m_Computation.SetLoop(0);
						}
					}
				}
				else if ( (std::strncmp("Atlas Loop: ", Line, 12)) == 0)
				{
					if (std::strlen(Line+12) != 0)
					{
						m_Computation.SetAtlasLoop(Line+12); 
						g_AtlasLoopDisp->value(Line+12);
						g_AtlasLoopDisp->position(g_AtlasLoopDisp->size());
					}
					else
					{
						m_Computation.SetAtlasLoop("");
						g_AtlasLoopDisp->value(NULL);
					}
				}
				else if ( (std::strncmp("Loop - Number of iterations: ", Line, 29)) == 0)
				{
					LoopIteration = atoi(Line+29);
					m_Computation.SetLoopIteration(LoopIteration);
					g_LoopIteration->value(LoopIteration);	
				}
			}
			if (mode == warping||mode == advancedParameters||mode == file)
			{
				if ( (std::strncmp("Warping Method: ", Line, 16)) == 0)
				{	
					if (std::strcmp(Line+16, "Classic") == 0)
					{
						m_Computation.SetClassicWarpingMethod(1);
						m_Computation.SetBRAINSDemonWarpMethod(0);
						g_ClassicWarpingButton->set();
						g_CoarseToFineWarpingButton->clear();
						g_BRAINSDemonWarpButton->clear();
						g_BRAINSDemonWarp->deactivate();
						g_Alpha->activate();
						g_Beta->activate();
						g_Gamma->activate();
						g_MaxPerturbation->activate();
						g_Scale1NbIterations->activate();
						g_NumBasis->activate();
						g_Scale4NbIterations->deactivate();
						g_Scale2NbIterations->deactivate();
					}
					else if (std::strcmp(Line+16, "Coarse-to-fine") == 0)
					{
						m_Computation.SetClassicWarpingMethod(0);
						m_Computation.SetBRAINSDemonWarpMethod(0);
						g_CoarseToFineWarpingButton->set();
						g_ClassicWarpingButton->clear();
						g_BRAINSDemonWarpButton->clear();
						g_BRAINSDemonWarp->deactivate();
						g_Alpha->activate();
						g_Beta->activate();
						g_Gamma->activate();
						g_MaxPerturbation->activate();
						g_Scale1NbIterations->activate();
						g_NumBasis->deactivate();
						g_Scale4NbIterations->activate();
						g_Scale2NbIterations->activate();
					}
					else if (std::strcmp(Line+16, "BRAINSDemonWarp") == 0)
					{
						m_Computation.SetClassicWarpingMethod(0);
						m_Computation.SetBRAINSDemonWarpMethod(1);
						g_CoarseToFineWarpingButton->clear();
						g_ClassicWarpingButton->clear();
						g_BRAINSDemonWarpButton->set();
						g_BRAINSDemonWarp->activate();
						g_Alpha->deactivate();
						g_Beta->deactivate();
						g_Gamma->deactivate();
						g_MaxPerturbation->deactivate();
						g_Scale1NbIterations->deactivate();
						g_NumBasis->deactivate();
						g_Scale4NbIterations->deactivate();
						g_Scale2NbIterations->deactivate();
					}
					else
						std::cerr<<"Error while reading parameter file: warping method incorrect!"<<std::endl;
				}
				else if ( (std::strncmp("Alpha: ", Line, 7)) == 0)
				{
					Alpha = atof(Line+7);
					m_Computation.SetAlpha(Alpha);
					g_Alpha->value(Alpha);	
				}
				else if ( (std::strncmp("Beta: ", Line, 6)) == 0)
				{
					Beta = atof(Line+6);
					m_Computation.SetBeta(Beta);
					g_Beta->value(Beta);	
				}
				else if ( (std::strncmp("Gamma: ", Line, 7)) == 0)
				{
					Gamma = atof(Line+7);
					m_Computation.SetGamma(Gamma);
					g_Gamma->value(Gamma);	
				}
				else if ( (std::strncmp("Max Perturbation: ", Line, 18)) == 0)
				{
					MaxPerturbation = atof(Line+18);
					m_Computation.SetMaxPerturbation(MaxPerturbation);
					g_MaxPerturbation->value(MaxPerturbation);	
				}
				else if ( (std::strncmp("Scale 4 - Number Of Iterations: ", Line, 32)) == 0)
				{
					Scale4NbIterations = atoi(Line+32);
					m_Computation.SetScale4NbIterations(Scale4NbIterations);
					g_Scale4NbIterations->value(Scale4NbIterations);	
				}
				else if ( (std::strncmp("Scale 2 - Number Of Iterations: ", Line, 32)) == 0)
				{
					Scale2NbIterations = atoi(Line+32);
					m_Computation.SetScale2NbIterations(Scale2NbIterations);
					g_Scale2NbIterations->value(Scale2NbIterations);	
				}
				else if ( (std::strncmp("Scale 1 - Number Of Iterations: ", Line, 32)) == 0)
				{
					Scale1NbIterations = atoi(Line+32);
					m_Computation.SetScale1NbIterations(Scale1NbIterations);
					g_Scale1NbIterations->value(Scale1NbIterations);	
				}
				else if ( (std::strncmp("Registration Filter Type: ", Line, 26)) == 0)
				{
					RegistrationFilterType =Line+26;
					m_Computation.SetRegistrationFilterType(RegistrationFilterType.c_str());	
					if (RegistrationFilterType=="Demons")
						g_RegistrationFilterType->value(0);
					else if(RegistrationFilterType=="FastSymmetricForces")
						g_RegistrationFilterType->value(1);
					else if(RegistrationFilterType=="Diffeomorphic")
						g_RegistrationFilterType->value(2);
					else if(RegistrationFilterType=="LogDemons")
						g_RegistrationFilterType->value(3);
					else if(RegistrationFilterType=="SymmetricLogDemons")
						g_RegistrationFilterType->value(4);
				}
				else if ( (std::strncmp("Deformation Field Smoothing Sigma: ", Line, 35)) == 0)
				{
					DeformationFieldSmoothingSigma = atof(Line+35);
					m_Computation.SetDeformationFieldSmoothingSigma(DeformationFieldSmoothingSigma);
					g_DeformationFieldSmoothingSigma->value(DeformationFieldSmoothingSigma);	
				}
				else if ( (std::strncmp("Pyramid Levels: ", Line, 16)) == 0)
				{
					PyramidLevels = atoi(Line+16);
					m_Computation.SetPyramidLevels(PyramidLevels);
					g_PyramidLevels->value(PyramidLevels);	
				}
				else if ( (std::strncmp("Moving Shrink Factors: ", Line, 23)) == 0)
				{
					MovingShrinkFactors =Line+23;
					m_Computation.SetMovingShrinkFactors(MovingShrinkFactors.c_str());
					g_MovingShrinkFactors->value(MovingShrinkFactors.c_str());	
				}
				else if ( (std::strncmp("Fixed Shrink Factors: ", Line, 22)) == 0)
				{
					FixedShrinkFactors =Line+22;
					m_Computation.SetFixedShrinkFactors(FixedShrinkFactors.c_str());
					g_FixedShrinkFactors->value(FixedShrinkFactors.c_str());	
				}
				else if ( (std::strncmp("Iteration Count Pyramid Levels: ", Line, 32)) == 0)
				{
					IterationCountPyramidLevels =Line+32;
					m_Computation.SetIterationCountPyramidLevels(IterationCountPyramidLevels.c_str());
					g_IterationCountPyramidLevels->value(IterationCountPyramidLevels.c_str());	
				}
	  // consistency with the first version of the tool
				else if ( (std::strncmp("Number Of Iterations: ", Line, 22)) == 0)
				{
					Scale1NbIterations = atoi(Line+22);
					m_Computation.SetScale1NbIterations(Scale1NbIterations);
					g_Scale1NbIterations->value(Scale1NbIterations);	
				}
				else if ( (std::strncmp("NumBasis: ", Line, 10)) == 0)
				{
					NumBasis = atof(Line+10);
					m_Computation.SetNumBasis(NumBasis);
					g_NumBasis->value(NumBasis);	
				}
			}
			if(mode == N4biasFieldCorrection||mode == advancedParameters||mode == file)
			{	
				if ((std::strncmp("N4 ITK Bias Field Correction: ", Line, 30)) == 0)
				{
					N4ITKBiasFieldCorrection= atoi(Line+30);
					if(mode == N4biasFieldCorrection)
					{
						g_N4ITKBiasFieldCorrectionButton->set();
						g_N4ParametersGroup->activate();
						g_N4AdvancedParametersGroup->activate();
						m_Computation.SetN4ITKBiasFieldCorrection(1);
					}
					else
					{
						if (N4ITKBiasFieldCorrection == 1)
						{
							g_N4ITKBiasFieldCorrectionButton->set();
							g_N4ParametersGroup->activate();
							g_N4AdvancedParametersGroup->activate();
							m_Computation.SetN4ITKBiasFieldCorrection(1);
						}
						else
						{
							g_N4ITKBiasFieldCorrectionButton->clear();
							g_N4ParametersGroup->deactivate();
							g_N4AdvancedParametersGroup->deactivate();
							m_Computation.SetN4ITKBiasFieldCorrection(0);
						}
					}
					
				}
				else if ( (std::strncmp("N4 Number of iterations: ", Line, 25)) == 0)
				{
					NbOfIterations = Line+25;
					m_Computation.SetNbOfIterations(NbOfIterations.c_str());
					g_NbOfIterations->value(NbOfIterations.c_str());	
				}
				else if ( (std::strncmp("N4 Spline distance: ", Line, 20)) == 0)
				{
					SplineDistance = atof(Line+20);
					m_Computation.SetSplineDistance(SplineDistance);
					g_SplineDistance->value(SplineDistance);	
				}
				else if ( (std::strncmp("N4 Shrink factor: ", Line, 18)) == 0)
				{
					ShrinkFactor = atoi(Line+18);
					m_Computation.SetShrinkFactor(ShrinkFactor);
					g_ShrinkFactor->value(ShrinkFactor);	
				}
				else if ( (std::strncmp("N4 Convergence threshold: ", Line, 26)) == 0)
				{
					ConvergenceThreshold = atof(Line+26);
					m_Computation.SetConvergenceThreshold(ConvergenceThreshold);
					g_ConvergenceThreshold->value(ConvergenceThreshold);	
				}
				else if ( (std::strncmp("N4 BSpline grid resolutions: ", Line, 29)) == 0)
				{
					BSplineGridResolutions = Line+29;
					m_Computation.SetBSplineGridResolutions(BSplineGridResolutions.c_str());
					g_BSplineGridResolutions->value(BSplineGridResolutions.c_str());	
				}
				else if ( (std::strncmp("N4 BSpline alpha: ", Line, 18)) == 0)
				{
					BSplineAlpha = atof(Line+18);
					m_Computation.SetBSplineAlpha(BSplineAlpha);
					g_BSplineAlpha->value(BSplineAlpha);	
				}
				else if ( (std::strncmp("N4 BSpline beta: ", Line, 17)) == 0)
				{
					BSplineBeta = atof(Line+17);
					m_Computation.SetBSplineBeta(BSplineBeta);
					g_BSplineBeta->value(BSplineBeta);	
				}
				else if ( (std::strncmp("N4 Histogram sharpening: ", Line, 25)) == 0)
				{
					HistogramSharpening = Line+25;
					m_Computation.SetHistogramSharpening(HistogramSharpening.c_str());
					g_HistogramSharpening->value(HistogramSharpening.c_str());	
				}
				else if ( (std::strncmp("N4 BSpline order: ", Line, 18)) == 0)
				{
					BSplineOrder =atoi(Line+18);
					m_Computation.SetBSplineOrder(BSplineOrder);
					g_BSplineOrder->value(BSplineOrder);	
				}
			}
		}
		fclose(ParameterFile);
	}
	else
	{
		std::cout<<"Error Opening File: "<<_FileName<<std::endl;
	}

	return IsParameterFileLoaded;
}

void AutoSegGUIControls::SetProcessDataDirectoryGUI()
{
	char *ProcessDataDirectory = NULL;
  
	ProcessDataDirectory = fl_dir_chooser("Set the Process Data Directory", NULL);
	if(ProcessDataDirectory != NULL)
	{
		CheckDirectoryName(ProcessDataDirectory);
		m_Computation.SetProcessDataDirectory(ProcessDataDirectory);
		g_ProcessDataDirectoryDisp->value(ProcessDataDirectory); 
		g_ProcessDataDirectoryDisp->position(g_ProcessDataDirectoryDisp->size());
	}
}

void AutoSegGUIControls::T2ButtonChecked()
{
	if (g_T2Button->value())
	{
		g_T2Title->activate();
		g_T2Disp->activate();
		m_Computation.SetT2Image(1);
	}
	else
	{
		g_T2Title->deactivate();
		g_T2Disp->deactivate(); 
		m_Computation.SetT2Image(0);
	}
}

void AutoSegGUIControls::PDButtonChecked()
{
	if (g_PDButton->value())
	{
		g_PDTitle->activate();
		g_PDDisp->activate();
		m_Computation.SetPDImage(1);
	}
	else
	{
		g_PDTitle->deactivate();
		g_PDDisp->deactivate();
		m_Computation.SetPDImage(0);
	}
}

void AutoSegGUIControls::AuxT1ButtonChecked()
{
	if (g_AuxT1Button->value())
	{
		m_Computation.SetAuxT1Image(1);
		m_Computation.SetAuxT2Image(0);
		m_Computation.SetAuxPDImage(0);
	}
	else
	{
		m_Computation.SetAuxT1Image(0);
	}
}

void AutoSegGUIControls::AuxT2ButtonChecked()
{
	if (g_AuxT2Button->value())
	{
		m_Computation.SetAuxT2Image(1);
		m_Computation.SetAuxT1Image(0);
		m_Computation.SetAuxPDImage(0);
	}
	else
	{
		m_Computation.SetAuxT2Image(0);
	}
}

void AutoSegGUIControls::AuxPDButtonChecked()
{
	if (g_AuxPDButton->value())
	{
		m_Computation.SetAuxPDImage(1);
		m_Computation.SetAuxT2Image(0);
		m_Computation.SetAuxT1Image(0);
	}
	else
	{
		m_Computation.SetAuxPDImage(0);
	}
}

void AutoSegGUIControls::Aux1ButtonChecked()
{
	if (g_Aux1Button->value())
	{
		g_Aux1Title->activate();
		g_Aux1Disp->activate();
		g_Aux1LabelDisp->activate();
		m_Computation.SetAux1Image(1);
		g_Aux2Button->activate();
		
		if (g_Aux2Button->value())
		{
			g_Aux2Title->activate();
			g_Aux2Disp->activate();
			g_Aux2LabelDisp->activate();
			m_Computation.SetAux2Image(1);
			g_Aux3Button->activate();
		}
		if ((g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux3Title->activate();
			g_Aux3Disp->activate();
			g_Aux3LabelDisp->activate();
			m_Computation.SetAux3Image(1);
			g_Aux4Button->activate();
		}
		if ((g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux4Title->activate();
			g_Aux4Disp->activate();
			g_Aux4LabelDisp->activate();
			m_Computation.SetAux4Image(1);
			g_Aux5Button->activate();
		}
		if ((g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux5Title->activate();
			g_Aux5Disp->activate();
			g_Aux5LabelDisp->activate();
			m_Computation.SetAux5Image(1);
			g_Aux6Button->activate();
		}
		if ((g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux6Title->activate();
			g_Aux6Disp->activate();
			g_Aux6LabelDisp->activate();
			m_Computation.SetAux6Image(1);
			g_Aux7Button->activate();
		}
		if ((g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux1Title->deactivate();
		g_Aux1Disp->deactivate();
		g_Aux1LabelDisp->deactivate();
		m_Computation.SetAux1Image(0);
		
		g_Aux2Title->deactivate();
		g_Aux2Disp->deactivate();
		g_Aux2LabelDisp->deactivate();
		m_Computation.SetAux2Image(0);
		g_Aux2Button->deactivate();
		g_Aux2Button->clear();
		
		g_Aux3Title->deactivate();
		g_Aux3Disp->deactivate();
		g_Aux3LabelDisp->deactivate();
		m_Computation.SetAux3Image(0);
		g_Aux3Button->deactivate();
		g_Aux3Button->clear();
		
		g_Aux4Title->deactivate();
		g_Aux4Disp->deactivate();
		g_Aux4LabelDisp->deactivate();
		m_Computation.SetAux4Image(0);
		g_Aux4Button->deactivate();
		g_Aux4Button->clear();
		
		g_Aux5Title->deactivate();
		g_Aux5Disp->deactivate();
		g_Aux5LabelDisp->deactivate();
		m_Computation.SetAux5Image(0);
		g_Aux5Button->deactivate();
		g_Aux5Button->clear();
		
		g_Aux6Title->deactivate();
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
		g_Aux6Button->deactivate();
		g_Aux6Button->clear();
		
		g_Aux7Title->deactivate();
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		
		g_Aux8Title->deactivate();
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
	}
}

void AutoSegGUIControls::Aux2ButtonChecked()
{
	if ((g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux2Title->activate();
		g_Aux2Disp->activate();
		g_Aux2LabelDisp->activate();
		m_Computation.SetAux2Image(1);
		g_Aux3Button->activate();
		
		if (g_Aux3Button->value())
		{
			g_Aux3Title->activate();
			g_Aux3Disp->activate();
			g_Aux3LabelDisp->activate();
			m_Computation.SetAux3Image(1);
			g_Aux4Button->activate();
		}
		if ((g_Aux4Button->value()) && (g_Aux3Button->value()))
		{
			g_Aux4Title->activate();
			g_Aux4Disp->activate();
			g_Aux4LabelDisp->activate();
			m_Computation.SetAux4Image(1);
			g_Aux5Button->activate();
		}
		if ((g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()))
		{
			g_Aux5Title->activate();
			g_Aux5Disp->activate();
			g_Aux5LabelDisp->activate();
			m_Computation.SetAux5Image(1);
			g_Aux6Button->activate();
		}
		if ((g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()))
		{
			g_Aux6Title->activate();
			g_Aux6Disp->activate();
			g_Aux6LabelDisp->activate();
			m_Computation.SetAux6Image(1);
			g_Aux7Button->activate();
		}
		if ((g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()))
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux2Title->deactivate();
		g_Aux2Disp->deactivate();
		g_Aux2LabelDisp->deactivate();
		m_Computation.SetAux2Image(0);
		
		g_Aux3Disp->deactivate();
		g_Aux3LabelDisp->deactivate();
		m_Computation.SetAux3Image(0);
		g_Aux3Button->deactivate();
		g_Aux3Button->clear();
		g_Aux3Title->deactivate();
		
		g_Aux4Disp->deactivate();
		g_Aux4LabelDisp->deactivate();
		m_Computation.SetAux4Image(0);
		g_Aux4Button->deactivate();
		g_Aux4Button->clear();
		g_Aux4Title->deactivate();
		
		g_Aux5Disp->deactivate();
		g_Aux5LabelDisp->deactivate();
		m_Computation.SetAux5Image(0);
		g_Aux5Button->deactivate();
		g_Aux5Button->clear();
		g_Aux5Title->deactivate();
		
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
		g_Aux6Button->deactivate();
		g_Aux6Button->clear();
		g_Aux6Title->deactivate();
		
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		g_Aux7Title->deactivate();
		
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux3ButtonChecked()
{
	if ((g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux3Title->activate();
		g_Aux3Disp->activate();
		g_Aux3LabelDisp->activate();
		m_Computation.SetAux3Image(1);
		g_Aux4Button->activate();
		
		if (g_Aux4Button->value())
		{
			g_Aux4Title->activate();
			g_Aux4Disp->activate();
			g_Aux4LabelDisp->activate();
			m_Computation.SetAux4Image(1);
			g_Aux5Button->activate();
		}
		if ((g_Aux5Button->value()) && (g_Aux4Button->value()))
		{
			g_Aux5Title->activate();
			g_Aux5Disp->activate();
			g_Aux5LabelDisp->activate();
			m_Computation.SetAux5Image(1);
			g_Aux6Button->activate();
		}
		if ((g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()))
		{
			g_Aux6Title->activate();
			g_Aux6Disp->activate();
			g_Aux6LabelDisp->activate();
			m_Computation.SetAux6Image(1);
			g_Aux7Button->activate();
		}
		if ((g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()))
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux3Title->deactivate();
		g_Aux3Disp->deactivate();
		g_Aux3LabelDisp->deactivate();
		m_Computation.SetAux3Image(0);
		
		g_Aux4Disp->deactivate();
		g_Aux4LabelDisp->deactivate();
		m_Computation.SetAux4Image(0);
		g_Aux4Button->deactivate();
		g_Aux4Button->clear();
		g_Aux4Title->deactivate();
		
		g_Aux5Disp->deactivate();
		g_Aux5LabelDisp->deactivate();
		m_Computation.SetAux5Image(0);
		g_Aux5Button->deactivate();
		g_Aux5Button->clear();
		g_Aux5Title->deactivate();
		
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
		g_Aux6Button->deactivate();
		g_Aux6Button->clear();
		g_Aux6Title->deactivate();
		
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		g_Aux7Title->deactivate();
		
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux4ButtonChecked()
{
	if ((g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux4Title->activate();
		g_Aux4Disp->activate();
		g_Aux4LabelDisp->activate();
		m_Computation.SetAux4Image(1);
		g_Aux5Button->activate();
		
		if (g_Aux5Button->value())
		{
			g_Aux5Title->activate();
			g_Aux5Disp->activate();
			g_Aux5LabelDisp->activate();
			m_Computation.SetAux5Image(1);
			g_Aux6Button->activate();
		}
		if ((g_Aux6Button->value()) && (g_Aux5Button->value()))
		{
			g_Aux6Title->activate();
			g_Aux6Disp->activate();
			g_Aux6LabelDisp->activate();
			m_Computation.SetAux6Image(1);
			g_Aux7Button->activate();
		}
		if ((g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()))
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux4Title->deactivate();
		g_Aux4Disp->deactivate();
		g_Aux4LabelDisp->deactivate();
		m_Computation.SetAux4Image(0);
				
		g_Aux5Disp->deactivate();
		g_Aux5LabelDisp->deactivate();
		m_Computation.SetAux5Image(0);
		g_Aux5Button->deactivate();
		g_Aux5Button->clear();
		g_Aux5Title->deactivate();
		
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
		g_Aux6Button->deactivate();
		g_Aux6Button->clear();
		g_Aux6Title->deactivate();
		
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		g_Aux7Title->deactivate();
		
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux5ButtonChecked()
{
	if ((g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux5Title->activate();
		g_Aux5Disp->activate();
		g_Aux5LabelDisp->activate();
		m_Computation.SetAux5Image(1);
		g_Aux6Button->activate();
		
		if (g_Aux6Button->value())
		{
			g_Aux6Title->activate();
			g_Aux6Disp->activate();
			g_Aux6LabelDisp->activate();
			m_Computation.SetAux6Image(1);
			g_Aux7Button->activate();
		}
		if ((g_Aux7Button->value()) && (g_Aux6Button->value()))
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8LabelDisp->activate();
			g_Aux8Disp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux5Title->deactivate();
		g_Aux5Disp->deactivate();
		g_Aux5LabelDisp->deactivate();
		m_Computation.SetAux5Image(0);
		
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
		g_Aux6Button->deactivate();
		g_Aux6Button->clear();
		g_Aux6Title->deactivate();
		
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		g_Aux7Title->deactivate();
		
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux6ButtonChecked()
{
	if ((g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux6Title->activate();
		g_Aux6Disp->activate();
		g_Aux6LabelDisp->activate();
		m_Computation.SetAux6Image(1);
		g_Aux7Button->activate();
		
		if (g_Aux7Button->value())
		{
			g_Aux7Title->activate();
			g_Aux7Disp->activate();
			g_Aux7LabelDisp->activate();
			m_Computation.SetAux7Image(1);
			g_Aux8Button->activate();
		}
		if ((g_Aux8Button->value()) && (g_Aux7Button->value()))
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux6Title->deactivate();
		g_Aux6Disp->deactivate();
		g_Aux6LabelDisp->deactivate();
		m_Computation.SetAux6Image(0);
				
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
		g_Aux7Button->deactivate();
		g_Aux7Button->clear();
		g_Aux7Title->deactivate();
		
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->deactivate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux7ButtonChecked()
{
	if ((g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux7Title->activate();
		g_Aux7Disp->activate();
		g_Aux7LabelDisp->activate();
		m_Computation.SetAux7Image(1);
		g_Aux8Button->activate();
		
		if (g_Aux8Button->value())
		{
			g_Aux8Title->activate();
			g_Aux8Disp->activate();
			g_Aux8LabelDisp->activate();
			m_Computation.SetAux8Image(1);
		}
	}
	else
	{
		g_Aux7Title->deactivate();
		g_Aux7Disp->deactivate();
		g_Aux7LabelDisp->deactivate();
		m_Computation.SetAux7Image(0);
				
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
		g_Aux8Button->activate();
		g_Aux8Button->clear();
		g_Aux8Title->deactivate();
	}
}

void AutoSegGUIControls::Aux8ButtonChecked()
{
	if ((g_Aux8Button->value()) && (g_Aux7Button->value()) && (g_Aux6Button->value()) && (g_Aux5Button->value()) && (g_Aux4Button->value()) && (g_Aux3Button->value()) && (g_Aux2Button->value()) && (g_Aux1Button->value()))
	{
		g_Aux8Title->activate();
		g_Aux8Disp->activate();
		g_Aux8LabelDisp->activate();
		m_Computation.SetAux8Image(1);
	}
	else
	{
		g_Aux8Title->deactivate();
		g_Aux8Disp->deactivate();
		g_Aux8LabelDisp->deactivate();
		m_Computation.SetAux8Image(0);
	}
}

void AutoSegGUIControls::SetDataDirectoryGUI()
{
	char *DataDirectory = NULL;
  
	DataDirectory = fl_dir_chooser("Set the Data Directory", NULL);
	if(DataDirectory != NULL)
	{
		CheckDirectoryName(DataDirectory);
		m_Computation.SetDataDirectory(DataDirectory);
		g_DataDirectoryDisp->value(DataDirectory);
		g_DataDirectoryDisp->position(g_DataDirectoryDisp->size());
	}
}

// Automatic Data Computation
void AutoSegGUIControls::ComputeDataGUI()
{
	bool InputChecked;
	InputChecked = CheckInputAutoDataSelection();
  
	if (InputChecked == false)
	{
		m_Computation.SetT1(g_T1Disp->value());
		if (g_T2Button->value())
			m_Computation.SetT2(g_T2Disp->value());
		else
			m_Computation.SetT2("");
		if (g_PDButton->value())
			m_Computation.SetPD(g_PDDisp->value());
		else
			m_Computation.SetPD("");

		InitBrowser();
		m_Computation.ComputeData();
		AddBrowserAutoData();
	}
}

// Automatic Aux Data Computation
void AutoSegGUIControls::ComputeAuxDataGUI()
{	
	if (g_Aux1Button->value())
	{
		m_Computation.SetAux1(g_Aux1Disp->value());
		m_Computation.SetAux1Label(g_Aux1LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux1("");
		m_Computation.SetAux1Label("");
	}
	if (g_Aux2Button->value())
	{
		m_Computation.SetAux2(g_Aux2Disp->value());
		m_Computation.SetAux2Label(g_Aux2LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux2("");
		m_Computation.SetAux2Label("");
	}
	if (g_Aux3Button->value())
	{
		m_Computation.SetAux3(g_Aux3Disp->value());
		m_Computation.SetAux3Label(g_Aux3LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux3("");
		m_Computation.SetAux3Label("");
	}
	if (g_Aux4Button->value())
	{
		m_Computation.SetAux4(g_Aux4Disp->value());
		m_Computation.SetAux4Label(g_Aux4LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux4("");
		m_Computation.SetAux4Label("");
	}
	if (g_Aux5Button->value())
	{
		m_Computation.SetAux5(g_Aux5Disp->value());
		m_Computation.SetAux5Label(g_Aux5LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux5("");
		m_Computation.SetAux5Label("");
	}
	if (g_Aux6Button->value())
	{
		m_Computation.SetAux6(g_Aux6Disp->value());
		m_Computation.SetAux6Label(g_Aux6LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux6("");
		m_Computation.SetAux6Label("");
	}
	if (g_Aux7Button->value())
	{
		m_Computation.SetAux7(g_Aux7Disp->value());
		m_Computation.SetAux7Label(g_Aux7LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux7("");
		m_Computation.SetAux7Label("");
	}
	if (g_Aux8Button->value())
	{
		m_Computation.SetAux8(g_Aux8Disp->value());
		m_Computation.SetAux8Label(g_Aux8LabelDisp->value());
	}
	else
	{
		m_Computation.SetAux8("");
		m_Computation.SetAux8Label("");
	}
	InitAuxBrowser();
	m_Computation.ComputeData();
	AddAuxBrowserAutoData();
}

bool AutoSegGUIControls::CheckInputAutoDataSelection()
{
	bool Warning = false;
	if (std::strlen(g_ProcessDataDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the process data directory...");
		Warning = true;
	}
	else if (std::strlen(g_DataDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the data directory...");
		Warning = true;
	} 
	else if (std::strlen(g_T1Disp->value()) == 0)
	{
		fl_message("Please, set the T1 files...");
		Warning = true;
	}
	else if ( (g_T2Button->value()) && (std::strlen(g_T2Disp->value()) == 0) )
	{
		fl_message("Please, set the T2 files...");
		Warning = true;
	}
	else if ( (g_PDButton->value()) && (std::strlen(g_PDDisp->value()) == 0) )
	{
		fl_message("Please, set the PD files...");
		Warning = true;
	}
	return Warning;
}

bool AutoSegGUIControls::CheckInputAutoAuxDataSelection()
{
	bool Warning = false;
	if (std::strlen(g_DataDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the data directory...");
		Warning = true;
	} 
	else if ( (g_Aux1Button->value()) && (std::strlen(g_Aux1Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux1 files...");
		Warning = true;
	}
	else if ( (g_Aux2Button->value()) && (std::strlen(g_Aux2Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux2 files...");
		Warning = true;
	}
	else if ( (g_Aux3Button->value()) && (std::strlen(g_Aux3Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux3 files...");
		Warning = true;
	}
	else if ( (g_Aux4Button->value()) && (std::strlen(g_Aux4Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux4 files...");
		Warning = true;
	}
	else if ( (g_Aux5Button->value()) && (std::strlen(g_Aux5Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux5 files...");
		Warning = true;
	}
	else if ( (g_Aux6Button->value()) && (std::strlen(g_Aux6Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux6 files...");
		Warning = true;
	}
	else if ( (g_Aux7Button->value()) && (std::strlen(g_Aux7Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux7 files...");
		Warning = true;
	}
	else if ( (g_Aux8Button->value()) && (std::strlen(g_Aux8Disp->value()) == 0) )
	{
		fl_message("Please, set the Aux8 files...");
		Warning = true;
	}
	return Warning;
}

//Browser Initialization
void AutoSegGUIControls::InitBrowser()
{
	g_DataBrowser->clear();

	if (m_BrowserWidths)
		delete[] m_BrowserWidths;

	if ( (g_T2Button->value()) && (g_PDButton->value()) )
	{
		m_BrowserWidths = new int [3];
		m_BrowserWidths[0] = 270;
		m_BrowserWidths[1] = 270;
		m_BrowserWidths[2] = 0;
		g_DataBrowser->column_widths(m_BrowserWidths);
		g_DataBrowser->showcolsep(1);
		g_DataBrowser->column_char(' ');
		g_DataBrowser->type(FL_MULTI_BROWSER);
		g_DataBrowser->clear();
		g_DataBrowser->add("@B20@b@cT1Image @B20@b@cT2Image @B20@b@cPDImage");
	}
	else
	{
		if ( (g_T2Button->value()) || (g_PDButton->value()) )
		{
			m_BrowserWidths = new int [2];
			m_BrowserWidths[0] = 400;
			m_BrowserWidths[1] = 0;
			g_DataBrowser->column_widths(m_BrowserWidths);
			g_DataBrowser->showcolsep(1);
			g_DataBrowser->column_char(' ');
			g_DataBrowser->type(FL_MULTI_BROWSER);
			g_DataBrowser->clear();
			if (g_T2Button->value())
				g_DataBrowser->add("@B20@b@cT1Image @B20@b@cT2Image");
			else
				g_DataBrowser->add("@B20@b@cT1Image @B20@b@cPDImage");
		}
		else
		{
			m_BrowserWidths = new int [1];
			m_BrowserWidths[0] = 0;
			g_DataBrowser->column_widths(m_BrowserWidths);
			g_DataBrowser->showcolsep(0);
			g_DataBrowser->column_char(' ');
			g_DataBrowser->type(FL_MULTI_BROWSER);
			g_DataBrowser->clear();
			g_DataBrowser->add("@B20@b@cT1Image");
		}
	}
}

//Browser Initialization
void AutoSegGUIControls::InitAuxBrowser()
{
	g_AuxDataBrowser->clear();

	if (m_AuxBrowserWidths)
		delete[] m_AuxBrowserWidths;
	if(g_AuxT1Button->value())
	{
		if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (g_Aux8Button->value()) )
		{
			m_AuxBrowserWidths = new int [9];
			m_AuxBrowserWidths[0] = 270;
			m_AuxBrowserWidths[1] = 270;
			m_AuxBrowserWidths[2] = 270;
			m_AuxBrowserWidths[3] = 270;
			m_AuxBrowserWidths[4] = 270;
			m_AuxBrowserWidths[5] = 270;
			m_AuxBrowserWidths[6] = 270;
			m_AuxBrowserWidths[7] = 270;
			m_AuxBrowserWidths[8] = 0;
			g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
			g_AuxDataBrowser->showcolsep(1);
			g_AuxDataBrowser->column_char(' ');
			g_AuxDataBrowser->type(FL_MULTI_BROWSER);
			g_AuxDataBrowser->clear();
			g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image @B20@b@cAux8Image");
		}
		else{
			if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) )
			{
				m_AuxBrowserWidths = new int [8];
				m_AuxBrowserWidths[0] = 270;
				m_AuxBrowserWidths[1] = 270;
				m_AuxBrowserWidths[2] = 270;
				m_AuxBrowserWidths[3] = 270;
				m_AuxBrowserWidths[4] = 270;
				m_AuxBrowserWidths[5] = 270;
				m_AuxBrowserWidths[6] = 270;
				m_AuxBrowserWidths[7] = 0;
				g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
				g_AuxDataBrowser->showcolsep(1);
				g_AuxDataBrowser->column_char(' ');
				g_AuxDataBrowser->type(FL_MULTI_BROWSER);
				g_AuxDataBrowser->clear();
				g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image");
			}
			else
			{
				if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) )
				{
					m_AuxBrowserWidths = new int [7];
					m_AuxBrowserWidths[0] = 270;
					m_AuxBrowserWidths[1] = 270;
					m_AuxBrowserWidths[2] = 270;
					m_AuxBrowserWidths[3] = 270;
					m_AuxBrowserWidths[4] = 270;
					m_AuxBrowserWidths[5] = 270;
					m_AuxBrowserWidths[6] = 0;
					g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
					g_AuxDataBrowser->showcolsep(1);
					g_AuxDataBrowser->column_char(' ');
					g_AuxDataBrowser->type(FL_MULTI_BROWSER);
					g_AuxDataBrowser->clear();
					g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image");
				}
				else
				{
					if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) )
					{
						m_AuxBrowserWidths = new int [6];
						m_AuxBrowserWidths[0] = 270;
						m_AuxBrowserWidths[1] = 270;
						m_AuxBrowserWidths[2] = 270;
						m_AuxBrowserWidths[3] = 270;
						m_AuxBrowserWidths[4] = 270;
						m_AuxBrowserWidths[5] = 0;
						g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
						g_AuxDataBrowser->showcolsep(1);
						g_AuxDataBrowser->column_char(' ');
						g_AuxDataBrowser->type(FL_MULTI_BROWSER);
						g_AuxDataBrowser->clear();
						g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image");
					}
					else
					{
						if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) )
						{
							m_AuxBrowserWidths = new int [5];
							m_AuxBrowserWidths[0] = 270;
							m_AuxBrowserWidths[1] = 270;
							m_AuxBrowserWidths[2] = 270;
							m_AuxBrowserWidths[3] = 270;
							m_AuxBrowserWidths[4] = 0;
							g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
							g_AuxDataBrowser->showcolsep(1);
							g_AuxDataBrowser->column_char(' ');
							g_AuxDataBrowser->type(FL_MULTI_BROWSER);
							g_AuxDataBrowser->clear();
							g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image");
						}
						else
						{
							if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) )
							{
								m_AuxBrowserWidths = new int [4];
								m_AuxBrowserWidths[0] = 270;
								m_AuxBrowserWidths[1] = 270;
								m_AuxBrowserWidths[2] = 270;
								m_AuxBrowserWidths[3] = 0;		
								g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
								g_AuxDataBrowser->showcolsep(1);
								g_AuxDataBrowser->column_char(' ');
								g_AuxDataBrowser->type(FL_MULTI_BROWSER);
								g_AuxDataBrowser->clear();
								g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image");
							}
							else
							{
								if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) )
								{
									m_AuxBrowserWidths = new int [3];
									m_AuxBrowserWidths[0] = 270;
									m_AuxBrowserWidths[1] = 270;
									m_AuxBrowserWidths[2] = 0;
									g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
									g_AuxDataBrowser->showcolsep(1);
									g_AuxDataBrowser->column_char(' ');
									g_AuxDataBrowser->type(FL_MULTI_BROWSER);
									g_AuxDataBrowser->clear();
									g_AuxDataBrowser->add("@B20@b@cOrigT1Image @B20@b@cAux1Image @B20@b@cAux2Image");
								}
								else
								{
									if ( (g_Aux1Button->value()) )
									{
										m_AuxBrowserWidths = new int [2];
										m_AuxBrowserWidths[0] = 400;
										m_AuxBrowserWidths[1] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(1);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cTOrig1Image @B20@b@cAux1Image");
									}
									else
									{
										m_AuxBrowserWidths = new int [1];
										m_AuxBrowserWidths[0] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(0);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cOrigT1Image");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if(g_AuxT2Button->value())
	{
		if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (g_Aux8Button->value()) )
		{
			m_AuxBrowserWidths = new int [9];
			m_AuxBrowserWidths[0] = 270;
			m_AuxBrowserWidths[1] = 270;
			m_AuxBrowserWidths[2] = 270;
			m_AuxBrowserWidths[3] = 270;
			m_AuxBrowserWidths[4] = 270;
			m_AuxBrowserWidths[5] = 270;
			m_AuxBrowserWidths[6] = 270;
			m_AuxBrowserWidths[7] = 270;
			m_AuxBrowserWidths[8] = 0;
			g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
			g_AuxDataBrowser->showcolsep(1);
			g_AuxDataBrowser->column_char(' ');
			g_AuxDataBrowser->type(FL_MULTI_BROWSER);
			g_AuxDataBrowser->clear();
			g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image @B20@b@cAux8Image");
		}
		else{
			if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) )
			{
				m_AuxBrowserWidths = new int [8];
				m_AuxBrowserWidths[0] = 270;
				m_AuxBrowserWidths[1] = 270;
				m_AuxBrowserWidths[2] = 270;
				m_AuxBrowserWidths[3] = 270;
				m_AuxBrowserWidths[4] = 270;
				m_AuxBrowserWidths[5] = 270;
				m_AuxBrowserWidths[6] = 270;
				m_AuxBrowserWidths[7] = 0;
				g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
				g_AuxDataBrowser->showcolsep(1);
				g_AuxDataBrowser->column_char(' ');
				g_AuxDataBrowser->type(FL_MULTI_BROWSER);
				g_AuxDataBrowser->clear();
				g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image");
			}
			else
			{
				if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) )
				{
					m_AuxBrowserWidths = new int [7];
					m_AuxBrowserWidths[0] = 270;
					m_AuxBrowserWidths[1] = 270;
					m_AuxBrowserWidths[2] = 270;
					m_AuxBrowserWidths[3] = 270;
					m_AuxBrowserWidths[4] = 270;
					m_AuxBrowserWidths[5] = 270;
					m_AuxBrowserWidths[6] = 0;
					g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
					g_AuxDataBrowser->showcolsep(1);
					g_AuxDataBrowser->column_char(' ');
					g_AuxDataBrowser->type(FL_MULTI_BROWSER);
					g_AuxDataBrowser->clear();
					g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image");
				}
				else
				{
					if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) )
					{
						m_AuxBrowserWidths = new int [6];
						m_AuxBrowserWidths[0] = 270;
						m_AuxBrowserWidths[1] = 270;
						m_AuxBrowserWidths[2] = 270;
						m_AuxBrowserWidths[3] = 270;
						m_AuxBrowserWidths[4] = 270;
						m_AuxBrowserWidths[5] = 0;
						g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
						g_AuxDataBrowser->showcolsep(1);
						g_AuxDataBrowser->column_char(' ');
						g_AuxDataBrowser->type(FL_MULTI_BROWSER);
						g_AuxDataBrowser->clear();
						g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image");
					}
					else
					{
						if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) )
						{
							m_AuxBrowserWidths = new int [5];
							m_AuxBrowserWidths[0] = 270;
							m_AuxBrowserWidths[1] = 270;
							m_AuxBrowserWidths[2] = 270;
							m_AuxBrowserWidths[3] = 270;
							m_AuxBrowserWidths[4] = 0;
							g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
							g_AuxDataBrowser->showcolsep(1);
							g_AuxDataBrowser->column_char(' ');
							g_AuxDataBrowser->type(FL_MULTI_BROWSER);
							g_AuxDataBrowser->clear();
							g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image");
						}
						else
						{
							if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) )
							{
								m_AuxBrowserWidths = new int [4];
								m_AuxBrowserWidths[0] = 270;
								m_AuxBrowserWidths[1] = 270;
								m_AuxBrowserWidths[2] = 270;
								m_AuxBrowserWidths[3] = 0;		
								g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
								g_AuxDataBrowser->showcolsep(1);
								g_AuxDataBrowser->column_char(' ');
								g_AuxDataBrowser->type(FL_MULTI_BROWSER);
								g_AuxDataBrowser->clear();
								g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image");
							}
							else
							{
								if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) )
								{
									m_AuxBrowserWidths = new int [3];
									m_AuxBrowserWidths[0] = 270;
									m_AuxBrowserWidths[1] = 270;
									m_AuxBrowserWidths[2] = 0;
									g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
									g_AuxDataBrowser->showcolsep(1);
									g_AuxDataBrowser->column_char(' ');
									g_AuxDataBrowser->type(FL_MULTI_BROWSER);
									g_AuxDataBrowser->clear();
									g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image @B20@b@cAux2Image");
								}
								else
								{
									if ( (g_Aux1Button->value()) )
									{
										m_AuxBrowserWidths = new int [2];
										m_AuxBrowserWidths[0] = 400;
										m_AuxBrowserWidths[1] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(1);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cOrigT2Image @B20@b@cAux1Image");
									}
									else
									{
										m_AuxBrowserWidths = new int [1];
										m_AuxBrowserWidths[0] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(0);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cOrigT2Image");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (g_AuxPDButton->value())
	{
		if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (g_Aux8Button->value()) )
		{
			m_AuxBrowserWidths = new int [9];
			m_AuxBrowserWidths[0] = 270;
			m_AuxBrowserWidths[1] = 270;
			m_AuxBrowserWidths[2] = 270;
			m_AuxBrowserWidths[3] = 270;
			m_AuxBrowserWidths[4] = 270;
			m_AuxBrowserWidths[5] = 270;
			m_AuxBrowserWidths[6] = 270;
			m_AuxBrowserWidths[7] = 270;
			m_AuxBrowserWidths[8] = 0;
			g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
			g_AuxDataBrowser->showcolsep(1);
			g_AuxDataBrowser->column_char(' ');
			g_AuxDataBrowser->type(FL_MULTI_BROWSER);
			g_AuxDataBrowser->clear();
			g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image @B20@b@cAux8Image");
		}
		else{
			if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) )
			{
				m_AuxBrowserWidths = new int [8];
				m_AuxBrowserWidths[0] = 270;
				m_AuxBrowserWidths[1] = 270;
				m_AuxBrowserWidths[2] = 270;
				m_AuxBrowserWidths[3] = 270;
				m_AuxBrowserWidths[4] = 270;
				m_AuxBrowserWidths[5] = 270;
				m_AuxBrowserWidths[6] = 270;
				m_AuxBrowserWidths[7] = 0;
				g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
				g_AuxDataBrowser->showcolsep(1);
				g_AuxDataBrowser->column_char(' ');
				g_AuxDataBrowser->type(FL_MULTI_BROWSER);
				g_AuxDataBrowser->clear();
				g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image @B20@b@cAux7Image");
			}
			else
			{
				if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) )
				{
					m_AuxBrowserWidths = new int [7];
					m_AuxBrowserWidths[0] = 270;
					m_AuxBrowserWidths[1] = 270;
					m_AuxBrowserWidths[2] = 270;
					m_AuxBrowserWidths[3] = 270;
					m_AuxBrowserWidths[4] = 270;
					m_AuxBrowserWidths[5] = 270;
					m_AuxBrowserWidths[6] = 0;
					g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
					g_AuxDataBrowser->showcolsep(1);
					g_AuxDataBrowser->column_char(' ');
					g_AuxDataBrowser->type(FL_MULTI_BROWSER);
					g_AuxDataBrowser->clear();
					g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image @B20@b@cAux6Image");
				}
				else
				{
					if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) )
					{
						m_AuxBrowserWidths = new int [6];
						m_AuxBrowserWidths[0] = 270;
						m_AuxBrowserWidths[1] = 270;
						m_AuxBrowserWidths[2] = 270;
						m_AuxBrowserWidths[3] = 270;
						m_AuxBrowserWidths[4] = 270;
						m_AuxBrowserWidths[5] = 0;
						g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
						g_AuxDataBrowser->showcolsep(1);
						g_AuxDataBrowser->column_char(' ');
						g_AuxDataBrowser->type(FL_MULTI_BROWSER);
						g_AuxDataBrowser->clear();
						g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image @B20@b@cAux5Image");
					}
					else
					{
						if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) )
						{
							m_AuxBrowserWidths = new int [5];
							m_AuxBrowserWidths[0] = 270;
							m_AuxBrowserWidths[1] = 270;
							m_AuxBrowserWidths[2] = 270;
							m_AuxBrowserWidths[3] = 270;
							m_AuxBrowserWidths[4] = 0;
							g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
							g_AuxDataBrowser->showcolsep(1);
							g_AuxDataBrowser->column_char(' ');
							g_AuxDataBrowser->type(FL_MULTI_BROWSER);
							g_AuxDataBrowser->clear();
							g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image @B20@b@cAux4Image");
						}
						else
						{
							if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) )
							{
								m_AuxBrowserWidths = new int [4];
								m_AuxBrowserWidths[0] = 270;
								m_AuxBrowserWidths[1] = 270;
								m_AuxBrowserWidths[2] = 270;
								m_AuxBrowserWidths[3] = 0;		
								g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
								g_AuxDataBrowser->showcolsep(1);
								g_AuxDataBrowser->column_char(' ');
								g_AuxDataBrowser->type(FL_MULTI_BROWSER);
								g_AuxDataBrowser->clear();
								g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image @B20@b@cAux3Image");
							}
							else
							{
								if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) )
								{
									m_AuxBrowserWidths = new int [3];
									m_AuxBrowserWidths[0] = 270;
									m_AuxBrowserWidths[1] = 270;
									m_AuxBrowserWidths[2] = 0;
									g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
									g_AuxDataBrowser->showcolsep(1);
									g_AuxDataBrowser->column_char(' ');
									g_AuxDataBrowser->type(FL_MULTI_BROWSER);
									g_AuxDataBrowser->clear();
									g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image @B20@b@cAux2Image");
								}
								else
								{
									if ( (g_Aux1Button->value()) )
									{
										m_AuxBrowserWidths = new int [2];
										m_AuxBrowserWidths[0] = 400;
										m_AuxBrowserWidths[1] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(1);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cOrigPDImage @B20@b@cAux1Image");
									}
									else
									{
										m_AuxBrowserWidths = new int [1];
										m_AuxBrowserWidths[0] = 0;
										g_AuxDataBrowser->column_widths(m_AuxBrowserWidths);
										g_AuxDataBrowser->showcolsep(0);
										g_AuxDataBrowser->column_char(' ');
										g_AuxDataBrowser->type(FL_MULTI_BROWSER);
										g_AuxDataBrowser->clear();
										g_AuxDataBrowser->add("@B20@b@cOrigPDImage");
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

// Add automatically computed data to browser
void AutoSegGUIControls::AddBrowserAutoData()
{
	m_Computation.SetManually(0);
	FILE* AutoDataFile;
	char Line[1536];
	char Data[1536];
	int Length;

	if ((AutoDataFile = fopen(m_Computation.GetDataFile(),"r")) != NULL)
	{
		while ( (fgets(Line,1536,AutoDataFile)) != NULL)
		{
			Length = std::strlen(Line);
			Line[Length-1] = '\0';
			if (std::strncmp(Line, "// ", 2) != 0)
			{
				CheckData(Line);
				CheckData2(Line);
				RightJustifyData(Line, Data);
				g_DataBrowser->add(Data);
			}
		}
		fclose(AutoDataFile);
	}
	else
	{
		std::cout<<"Error Opening File: "<<m_Computation.GetDataFile()<<std::endl;
		exit(-1);
	}
}

// Add automatically computed Auxdata to Auxbrowser
void AutoSegGUIControls::AddAuxBrowserAutoData()
{
	FILE* AutoAuxDataFile;
	char Line[1536];
	char Data[1536];
	int Length;

	if ((AutoAuxDataFile = fopen(m_Computation.GetAuxDataFile(),"r")) != NULL)
	{
		while ( (fgets(Line,1536,AutoAuxDataFile)) != NULL)
		{
			Length = std::strlen(Line);
			Line[Length-1] = '\0';
			if (std::strncmp(Line, "// ", 2) != 0)
			{
				CheckData(Line);
				CheckData2(Line);
				RightJustifyAuxData(Line, Data);
				g_AuxDataBrowser->add(Data);
				CheckData2(Line);
			}
		}
		fclose(AutoAuxDataFile);
	}
	else
	{
		std::cout<<"Error Opening File: "<<m_Computation.GetAuxDataFile()<<std::endl;
		exit(-1);
	}
}

//Replace strings "//" by '/'
void AutoSegGUIControls::CheckData(char *_Line)
{
	std::string Line = _Line;
	std::string tmp;
	std::string::size_type loc;
	int InLoop = 1;
	while (InLoop == 1)
	{
		loc = Line.find( "//", 0);
		if( loc != std::string::npos )
		{
			tmp.assign(Line,0,loc+1);
			tmp.append(Line,loc+2, Line.size()-loc);
			Line = tmp;
		}
		else
			InLoop = 0;
	}  
	std::strcpy(_Line,Line.c_str());
}

//Deals with "/../" strings
void AutoSegGUIControls::CheckData2(char *_Line)
{
	std::string Line = _Line;
	std::string tmp1, tmp2;
	std::string::size_type loc1,loc2;
	int InLoop = 1;
	while (InLoop == 1)
	{
		loc1 = Line.find( "/../", 0);
		if( loc1 != std::string::npos )
		{
			tmp1.assign(Line,0,loc1);
			tmp2.assign(Line,loc1+4,Line.size()-loc1-4);
			loc2 = tmp1.find_last_of("/",tmp1.size());
			tmp1.append("/");
			tmp1.replace(loc2+1,tmp2.size(),tmp2);	  
			Line = tmp1;
		}
		else
			InLoop = 0;
	}  
	std::strcpy(_Line, Line.c_str());
}

// Add data manually to browser
void AutoSegGUIControls::AddDataGUI()
{
	char Line[1536]; 

	std::strcpy(Line, "");
	
	AddDataGUIControls AddData(g_T2Button->value(), g_PDButton->value());
	AddData.g_MainWindow->show();
	while(AddData.g_MainWindow->shown())
		Fl::wait();
	
	if ( (!g_T2Button->value()) && (!g_PDButton->value()) && (std::strlen(AddData.GetT1File())!= 0) ) 
		std::strcpy(Line, AddData.GetT1File());
	else if (std::strlen(AddData.GetT1File())!= 0)
	{
		std::strcpy(Line, "@r");
		std::strcat(Line, AddData.GetT1File());
	}

	if ( (g_T2Button->value()) && (g_PDButton->value()) && (std::strlen(AddData.GetT2File())!= 0)  && (std::strlen(AddData.GetPDFile())!= 0)  ) 
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddData.GetT2File());
		std::strcat(Line, " ");
		std::strcat(Line, AddData.GetPDFile());
	}
	else if ( (g_T2Button->value()) && (std::strlen(AddData.GetT2File())!= 0) )
	{
		std::strcat(Line, " ");
		std::strcat(Line, AddData.GetT2File());
	}
	else if (std::strlen(AddData.GetPDFile())!= 0)
	{
		std::strcat(Line, " ");
		std::strcat(Line, AddData.GetPDFile()); 
	}

	if (g_DataBrowser->size() < 2)
		InitBrowser();  
	if (std::strlen(Line) != 0)
		g_DataBrowser->add(Line);	
}

// Add data manually to Auxbrowser
void AutoSegGUIControls::AddAuxDataGUI()
{
	char Line[1536];

	m_Computation.SetManually(1);
	
	std::strcpy(Line, "");
	
	AddAuxDataGUIControls AddAuxData(g_AuxT1Button->value(), g_AuxT2Button->value(), g_AuxPDButton->value(), g_Aux1Button->value(), g_Aux2Button->value(), g_Aux3Button->value(), g_Aux4Button->value(), g_Aux5Button->value(), g_Aux6Button->value(), g_Aux7Button->value(), g_Aux8Button->value());
	AddAuxData.g_MainWindow->show();
	while(AddAuxData.g_MainWindow->shown())
		Fl::wait();
	
	if ( (!g_Aux1Button->value()) && (!g_Aux2Button->value()) && (!g_Aux3Button->value()) && (!g_Aux4Button->value()) && (!g_Aux5Button->value())  && (!g_Aux6Button->value()) && (!g_Aux7Button->value()) && (!g_Aux8Button->value()) && (std::strlen(AddAuxData.GetAuxT1File())!= 0) ) 
		if (g_AuxT1Button->value())
			std::strcpy(Line, AddAuxData.GetAuxT1File());
		if (g_AuxT2Button->value())
			std::strcpy(Line, AddAuxData.GetAuxT2File());
		if (g_AuxPDButton->value())
			std::strcpy(Line, AddAuxData.GetAuxPDFile());
	else if (std::strlen(AddAuxData.GetAuxT1File())!= 0)
	{
		std::strcpy(Line, "@r");
		std::strcat(Line, AddAuxData.GetAuxT1File());
	}
	else if (std::strlen(AddAuxData.GetAuxT2File())!= 0)
	{
		std::strcpy(Line, "@r");
		std::strcat(Line, AddAuxData.GetAuxT2File());
	}
	else if (std::strlen(AddAuxData.GetAuxPDFile())!= 0)
	{
		std::strcpy(Line, "@r");
		std::strcat(Line, AddAuxData.GetAuxPDFile());
	}
	if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (g_Aux8Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0)   && (std::strlen(AddAuxData.GetAux4File())!= 0)  && (std::strlen(AddAuxData.GetAux5File())!= 0)  && (std::strlen(AddAuxData.GetAux6File())!= 0)  && (std::strlen(AddAuxData.GetAux7File())!= 0)  && (std::strlen(AddAuxData.GetAux8File())!= 0) )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux3File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux4File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux5File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux6File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux7File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux8File());
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0)   && (std::strlen(AddAuxData.GetAux4File())!= 0)  && (std::strlen(AddAuxData.GetAux5File())!= 0)  && (std::strlen(AddAuxData.GetAux6File())!= 0)  && (std::strlen(AddAuxData.GetAux7File())!= 0)  )
	{
		std::strcat(Line, " @r");	
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux3File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux4File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux5File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux6File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux7File());
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0)   && (std::strlen(AddAuxData.GetAux4File())!= 0)  && (std::strlen(AddAuxData.GetAux5File())!= 0)  && (std::strlen(AddAuxData.GetAux6File())!= 0) )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux3File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux4File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux5File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux6File());
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0)   && (std::strlen(AddAuxData.GetAux4File())!= 0)  && (std::strlen(AddAuxData.GetAux5File())!= 0) )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux3File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux4File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux5File());
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value())  && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0)   && (std::strlen(AddAuxData.GetAux4File())!= 0) )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux3File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux4File());
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  && (std::strlen(AddAuxData.GetAux3File())!= 0) )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux2File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux3File());
	}
	
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0)  && (std::strlen(AddAuxData.GetAux2File())!= 0)  )
	{
		std::strcat(Line, " @r");
		std::strcat(Line, AddAuxData.GetAux1File());
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux2File());
	}
	else if ( (g_Aux1Button->value()) && (std::strlen(AddAuxData.GetAux1File())!= 0) )
	{
		std::strcat(Line, " ");
		std::strcat(Line, AddAuxData.GetAux1File());
	}

	if (g_AuxDataBrowser->size() < 2)
		InitAuxBrowser();  
	if (std::strlen(Line) != 0)
		g_AuxDataBrowser->add(Line);
}

// Remove data manually from browser
void AutoSegGUIControls::RemoveDataGUI()
{
	int line = 2;
	while (line <= g_DataBrowser->size())
	{
		if (g_DataBrowser->selected(line))
			g_DataBrowser->remove(line);
		else
			line++;
	}
}

// Remove data manually from Auxbrowser
void AutoSegGUIControls::RemoveAuxDataGUI()
{
	int line = 2;
	while (line <= g_AuxDataBrowser->size())
	{
		if (g_AuxDataBrowser->selected(line))
			g_AuxDataBrowser->remove(line);
		else
			line++;
	}
}

// Clear browser
void AutoSegGUIControls::ClearDataGUI()
{
	InitBrowser();
}

void AutoSegGUIControls::ClearAuxDataGUI()
{
	InitAuxBrowser();
}

void AutoSegGUIControls::SetCommonCoordinateImageGUI()
{
	char *CommonCoordinateImage = NULL;

	CommonCoordinateImage = fl_file_chooser("Set the Common Coordinate Image","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);
	if(CommonCoordinateImage != NULL)
	{
		m_Computation.SetCommonCoordinateImage(CommonCoordinateImage);
		g_CommonCoordinateImageDisp->value(CommonCoordinateImage);
		g_CommonCoordinateImageDisp->position(g_CommonCoordinateImageDisp->size());
	}
}

void AutoSegGUIControls::SetTissueSegmentationAtlasDirectoryGUI()
{
	char *TissueSegmentationAtlasDirectory = NULL;

	TissueSegmentationAtlasDirectory = fl_dir_chooser("Set the Tissue Segmentation Atlas Directory",NULL);
	if(TissueSegmentationAtlasDirectory != NULL)
	{
		CheckDirectoryName(TissueSegmentationAtlasDirectory);
		m_Computation.SetTissueSegmentationAtlasDirectory(TissueSegmentationAtlasDirectory);
		g_TissueSegmentationAtlasDirectoryDisp->value(TissueSegmentationAtlasDirectory);  
		g_TissueSegmentationAtlasDirectoryDisp->position(g_TissueSegmentationAtlasDirectoryDisp->size());
	}
}

void AutoSegGUIControls::SetROIAtlasFileGUI()
{
	char *ROIAtlasFile = NULL;

	ROIAtlasFile = fl_file_chooser("Set the ROI Atlas File", "Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);
	if(ROIAtlasFile != NULL)
	{
		m_Computation.SetROIAtlasFile(ROIAtlasFile);
		g_ROIAtlasFileDisp->value(ROIAtlasFile);
		g_ROIAtlasFileDisp->position(g_ROIAtlasFileDisp->size());
	}
}

void AutoSegGUIControls::SetLoopGUI()
{
  char *AtlasLoopDirectory = NULL;
  
  AtlasLoopDirectory = fl_dir_chooser("Set the Atlas loop directory",NULL);
  if(AtlasLoopDirectory != NULL)
    {
      CheckDirectoryName(AtlasLoopDirectory);
      m_Computation.SetAtlasLoop(AtlasLoopDirectory);
      g_AtlasLoopDisp->value(AtlasLoopDirectory);
      g_AtlasLoopDisp->position(g_AtlasLoopDisp->size());
    }
}

void AutoSegGUIControls::TissueSegmentationAtlasT1ButtonToggled()
{
	g_TissueSegmentationAtlasT1Button->set();
	g_TissueSegmentationAtlasT2Button->clear();

	m_Computation.SetTissueSegmentationAtlasType("T1");
}

void AutoSegGUIControls::TissueSegmentationAtlasT2ButtonToggled()
{
	g_TissueSegmentationAtlasT2Button->set();
	g_TissueSegmentationAtlasT1Button->clear();
  
	m_Computation.SetTissueSegmentationAtlasType("T2");
}

void AutoSegGUIControls::CommonCoordinateImageT1ButtonToggled()
{
	g_CommonCoordinateImageT1Button->set();
	g_CommonCoordinateImageT2Button->clear();
  
	m_Computation.SetCommonCoordinateImageType("T1");
}

void AutoSegGUIControls::CommonCoordinateImageT2ButtonToggled()
{
	g_CommonCoordinateImageT2Button->set();
	g_CommonCoordinateImageT1Button->clear();

	m_Computation.SetCommonCoordinateImageType("T2");
}

void AutoSegGUIControls::SetAmygdalaLeftGUI()
{
	char *AmygdalaLeft = NULL;

	AmygdalaLeft = fl_file_chooser("Set the Amygdala File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(AmygdalaLeft != NULL)
	{
		m_Computation.SetAmygdalaLeft(AmygdalaLeft);
		g_AmygdalaLeftDisp->value(AmygdalaLeft); 
		g_AmygdalaLeftDisp->position(g_AmygdalaLeftDisp->size());
	}
}

void AutoSegGUIControls::SetAmygdalaRightGUI()
{
	char *AmygdalaRight = NULL;

	AmygdalaRight = fl_file_chooser("Set the Amygdala File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(AmygdalaRight != NULL)
	{
		m_Computation.SetAmygdalaRight(AmygdalaRight);
		g_AmygdalaRightDisp->value(AmygdalaRight); 
		g_AmygdalaRightDisp->position(g_AmygdalaRightDisp->size());
	}
}

void AutoSegGUIControls::SetCaudateLeftGUI()
{
	char *CaudateLeft = NULL;

	CaudateLeft = fl_file_chooser("Set the Caudate File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(CaudateLeft != NULL)
	{
		m_Computation.SetCaudateLeft(CaudateLeft);
		g_CaudateLeftDisp->value(CaudateLeft); 
		g_CaudateLeftDisp->position(g_CaudateLeftDisp->size());
	}
}

void AutoSegGUIControls::SetCaudateRightGUI()
{
	char *CaudateRight = NULL;

	CaudateRight = fl_file_chooser("Set the Caudate File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(CaudateRight != NULL)
	{
		m_Computation.SetCaudateRight(CaudateRight);
		g_CaudateRightDisp->value(CaudateRight); 
		g_CaudateRightDisp->position(g_CaudateRightDisp->size());
	}
}

void AutoSegGUIControls::SetHippocampusLeftGUI()
{
	char *HippocampusLeft = NULL;

	HippocampusLeft = fl_file_chooser("Set the Hippocampus File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(HippocampusLeft != NULL)
	{
		m_Computation.SetHippocampusLeft(HippocampusLeft);
		g_HippocampusLeftDisp->value(HippocampusLeft); 
		g_HippocampusLeftDisp->position(g_HippocampusLeftDisp->size());
	}
}

void AutoSegGUIControls::SetHippocampusRightGUI()
{
	char *HippocampusRight = NULL;

	HippocampusRight = fl_file_chooser("Set the Hippocampus File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(HippocampusRight != NULL)
	{
		m_Computation.SetHippocampusRight(HippocampusRight);
		g_HippocampusRightDisp->value(HippocampusRight); 
		g_HippocampusRightDisp->position(g_HippocampusRightDisp->size());
	}
}

void AutoSegGUIControls::SetPallidusLeftGUI()
{
	char *PallidusLeft = NULL;

	PallidusLeft = fl_file_chooser("Set the Pallidus File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(PallidusLeft != NULL)
	{
		m_Computation.SetPallidusLeft(PallidusLeft);
		g_PallidusLeftDisp->value(PallidusLeft); 
		g_PallidusLeftDisp->position(g_PallidusLeftDisp->size());
	}
}

void AutoSegGUIControls::SetPallidusRightGUI()
{
	char *PallidusRight = NULL;

	PallidusRight = fl_file_chooser("Set the Pallidus File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(PallidusRight != NULL)
	{
		m_Computation.SetPallidusRight(PallidusRight);
		g_PallidusRightDisp->value(PallidusRight); 
		g_PallidusRightDisp->position(g_PallidusRightDisp->size());
	}
}

void AutoSegGUIControls::SetPutamenLeftGUI()
{
	char *PutamenLeft = NULL;

	PutamenLeft = fl_file_chooser("Set the Putamen File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(PutamenLeft != NULL)
	{
		m_Computation.SetPutamenLeft(PutamenLeft);
		g_PutamenLeftDisp->value(PutamenLeft); 
		g_PutamenLeftDisp->position(g_PutamenLeftDisp->size());
	}
}

void AutoSegGUIControls::SetPutamenRightGUI()
{
	char *PutamenRight = NULL;

	PutamenRight = fl_file_chooser("Set the Putamen File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(PutamenRight != NULL)
	{
		m_Computation.SetPutamenRight(PutamenRight);
		g_PutamenRightDisp->value(PutamenRight); 
		g_PutamenRightDisp->position(g_PutamenRightDisp->size());
	}
}

void AutoSegGUIControls::SetLateralVentricleLeftGUI()
{
	char *LateralVentricleLeft = NULL;

	LateralVentricleLeft = fl_file_chooser("Set the Lateral Ventricle File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(LateralVentricleLeft != NULL)
	{
		m_Computation.SetLateralVentricleLeft(LateralVentricleLeft);
		g_LateralVentricleLeftDisp->value(LateralVentricleLeft); 
		g_LateralVentricleLeftDisp->position(g_LateralVentricleLeftDisp->size());
	}
}

void AutoSegGUIControls::SetLateralVentricleRightGUI()
{
	char *LateralVentricleRight = NULL;

	LateralVentricleRight = fl_file_chooser("Set the Lateral Ventricle File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL);  
	if(LateralVentricleRight != NULL)
	{
		m_Computation.SetLateralVentricleRight(LateralVentricleRight);
		g_LateralVentricleRightDisp->value(LateralVentricleRight); 
		g_LateralVentricleRightDisp->position(g_LateralVentricleRightDisp->size());
	}
}

void AutoSegGUIControls::AllStructuresButtonChecked()
{
	if (g_AllStructuresButton->value())
	{
		g_AmygdalaLeftButton->set();
		g_AmygdalaRightButton->set();
		g_CaudateLeftButton->set();
		g_CaudateRightButton->set();
		g_HippocampusLeftButton->set();
		g_HippocampusRightButton->set();
		g_PallidusLeftButton->set();
		g_PallidusRightButton->set();
		g_PutamenLeftButton->set();
		g_PutamenRightButton->set();
		g_LateralVentricleLeftButton->set();
		g_LateralVentricleRightButton->set();     

		g_AmygdalaLeftDisp->activate();
		g_AmygdalaRightDisp->activate();
		g_CaudateLeftDisp->activate();
		g_CaudateRightDisp->activate();
		g_HippocampusLeftDisp->activate();
		g_HippocampusRightDisp->activate();
		g_PallidusLeftDisp->activate();
		g_PallidusRightDisp->activate();
		g_PutamenLeftDisp->activate();
		g_PutamenRightDisp->activate();
		g_LateralVentricleLeftDisp->activate();
		g_LateralVentricleRightDisp->activate();
	}
	else
	{
		g_AmygdalaLeftButton->clear();
		g_AmygdalaRightButton->clear();
		g_CaudateLeftButton->clear();
		g_CaudateRightButton->clear();
		g_HippocampusLeftButton->clear();
		g_HippocampusRightButton->clear();
		g_PallidusLeftButton->clear();
		g_PallidusRightButton->clear();
		g_PutamenLeftButton->clear();
		g_PutamenRightButton->clear();
		g_LateralVentricleLeftButton->clear();
		g_LateralVentricleRightButton->clear();

		g_AmygdalaLeftDisp->deactivate();
		g_AmygdalaRightDisp->deactivate();
		g_CaudateLeftDisp->deactivate();
		g_CaudateRightDisp->deactivate();
		g_HippocampusLeftDisp->deactivate();
		g_HippocampusRightDisp->deactivate();
		g_PallidusLeftDisp->deactivate();
		g_PallidusRightDisp->deactivate();
		g_PutamenLeftDisp->deactivate();
		g_PutamenRightDisp->deactivate();
		g_LateralVentricleLeftDisp->deactivate();
		g_LateralVentricleRightDisp->deactivate();     
	}
}

void AutoSegGUIControls::AmygdalaLeftButtonChecked()
{
	if (g_AmygdalaLeftButton->value())
		g_AmygdalaLeftDisp->activate();
	else
		g_AmygdalaLeftDisp->deactivate();
}

void AutoSegGUIControls::AmygdalaRightButtonChecked()
{
	if (g_AmygdalaRightButton->value())
		g_AmygdalaRightDisp->activate();
	else
		g_AmygdalaRightDisp->deactivate();
}

void AutoSegGUIControls::CaudateLeftButtonChecked()
{
	if (g_CaudateLeftButton->value())
		g_CaudateLeftDisp->activate();
	else
		g_CaudateLeftDisp->deactivate();
}

void AutoSegGUIControls::CaudateRightButtonChecked()
{
	if (g_CaudateRightButton->value())
		g_CaudateRightDisp->activate();
	else
		g_CaudateRightDisp->deactivate();
}

void AutoSegGUIControls::HippocampusLeftButtonChecked()
{
	if (g_HippocampusLeftButton->value())
		g_HippocampusLeftDisp->activate();
	else
		g_HippocampusLeftDisp->deactivate();
}

void AutoSegGUIControls::HippocampusRightButtonChecked()
{
	if (g_HippocampusRightButton->value())
		g_HippocampusRightDisp->activate();
	else
		g_HippocampusRightDisp->deactivate();
}

void AutoSegGUIControls::PallidusLeftButtonChecked()
{
	if (g_PallidusLeftButton->value())
		g_PallidusLeftDisp->activate();
	else
		g_PallidusLeftDisp->deactivate();
}

void AutoSegGUIControls::PallidusRightButtonChecked()
{
	if (g_PallidusRightButton->value())
		g_PallidusRightDisp->activate();
	else
		g_PallidusRightDisp->deactivate();
}

void AutoSegGUIControls::PutamenLeftButtonChecked()
{
	if (g_PutamenLeftButton->value())
		g_PutamenLeftDisp->activate();
	else
		g_PutamenLeftDisp->deactivate();
}

void AutoSegGUIControls::PutamenRightButtonChecked()
{
	if (g_PutamenRightButton->value())
		g_PutamenRightDisp->activate();
	else
		g_PutamenRightDisp->deactivate();
}

void AutoSegGUIControls::LateralVentricleLeftButtonChecked()
{
	if (g_LateralVentricleLeftButton->value())
		g_LateralVentricleLeftDisp->activate();
	else
		g_LateralVentricleLeftDisp->deactivate();
}

void AutoSegGUIControls::LateralVentricleRightButtonChecked()
{
	if (g_LateralVentricleRightButton->value())
		g_LateralVentricleRightDisp->activate();
	else
		g_LateralVentricleRightDisp->deactivate();
}

void AutoSegGUIControls::SetROIFile1GUI()
{
	char *ROIFile1 = NULL;

	ROIFile1 = fl_file_chooser("Set the ROI File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ROIFile1 != NULL)
	{
		m_Computation.SetROIFile1(ROIFile1);
		g_ROIFile1Disp->value(ROIFile1); 
		g_ROIFile1Disp->position(g_ROIFile1Disp->size());
	}
}

void AutoSegGUIControls::SetROIFile2GUI()
{
	char *ROIFile2 = NULL;

	ROIFile2 = fl_file_chooser("Set the ROI File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ROIFile2 != NULL)
	{
		m_Computation.SetROIFile2(ROIFile2);
		g_ROIFile2Disp->value(ROIFile2); 
		g_ROIFile2Disp->position(g_ROIFile2Disp->size());
	}
}

void AutoSegGUIControls::SetROIFile3GUI()
{
	char *ROIFile3 = NULL;

	ROIFile3 = fl_file_chooser("Set the ROI File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ROIFile3 != NULL)
	{
		m_Computation.SetROIFile3(ROIFile3);
		g_ROIFile3Disp->value(ROIFile3); 
		g_ROIFile3Disp->position(g_ROIFile3Disp->size());
	}
}

void AutoSegGUIControls::SetROIFile4GUI()
{
	char *ROIFile4 = NULL;

	ROIFile4 = fl_file_chooser("Set the ROI File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ROIFile4 != NULL)
	{
		m_Computation.SetROIFile4(ROIFile4);
		g_ROIFile4Disp->value(ROIFile4); 
		g_ROIFile4Disp->position(g_ROIFile4Disp->size());
	}
}

void AutoSegGUIControls::SetROIFile5GUI()
{
	char *ROIFile5 = NULL;

	ROIFile5 = fl_file_chooser("Set the ROI File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ROIFile5 != NULL)
	{
		m_Computation.SetROIFile5(ROIFile5);
		g_ROIFile5Disp->value(ROIFile5); 
		g_ROIFile5Disp->position(g_ROIFile5Disp->size());
	}
}

void AutoSegGUIControls::ROIFile1ButtonChecked()
{
	if (g_ROIFile1Button->value())
		g_ROIFile1Disp->activate();
	else
		g_ROIFile1Disp->deactivate();
}

void AutoSegGUIControls::ROIFile2ButtonChecked()
{
	if (g_ROIFile2Button->value())
		g_ROIFile2Disp->activate();
	else
		g_ROIFile2Disp->deactivate();
}

void AutoSegGUIControls::ROIFile3ButtonChecked()
{
	if (g_ROIFile3Button->value())
		g_ROIFile3Disp->activate();
	else
		g_ROIFile3Disp->deactivate();
}

void AutoSegGUIControls::ROIFile4ButtonChecked()
{
	if (g_ROIFile4Button->value())
		g_ROIFile4Disp->activate();
	else
		g_ROIFile4Disp->deactivate();
}

void AutoSegGUIControls::ROIFile5ButtonChecked()
{
	if (g_ROIFile5Button->value())
		g_ROIFile5Disp->activate();
	else
		g_ROIFile5Disp->deactivate();
}

void AutoSegGUIControls::SetParcellationFile1GUI()
{
	char *ParcellationFile1 = NULL;

	ParcellationFile1 = fl_file_chooser("Set the Parcellation File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ParcellationFile1 != NULL)
	{
		m_Computation.SetParcellationFile1(ParcellationFile1);
		g_ParcellationFile1Disp->value(ParcellationFile1); 
		g_ParcellationFile1Disp->position(g_ParcellationFile1Disp->size());
	}
}

void AutoSegGUIControls::SetParcellationFile2GUI()
{
	char *ParcellationFile2 = NULL;

	ParcellationFile2 = fl_file_chooser("Set the Parcellation File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ParcellationFile2 != NULL)
	{
		m_Computation.SetParcellationFile2(ParcellationFile2);
		g_ParcellationFile2Disp->value(ParcellationFile2); 
		g_ParcellationFile2Disp->position(g_ParcellationFile2Disp->size());
	}
}

void AutoSegGUIControls::SetParcellationFile3GUI()
{
	char *ParcellationFile3 = NULL;

	ParcellationFile3 = fl_file_chooser("Set the Parcellation File","Images (*.{gipl,gipl.gz,mhd,mha,hdr,nhdr,nrrd})",NULL); 
	if(ParcellationFile3 != NULL)
	{
		m_Computation.SetParcellationFile3(ParcellationFile3);
		g_ParcellationFile3Disp->value(ParcellationFile3); 
		g_ParcellationFile3Disp->position(g_ParcellationFile3Disp->size());
	}
}

void AutoSegGUIControls::ParcellationFile1ButtonChecked()
{
	if (g_ParcellationFile1Button->value())
		g_ParcellationFile1Disp->activate();
	else
		g_ParcellationFile1Disp->deactivate();
}

void AutoSegGUIControls::ParcellationFile2ButtonChecked()
{
	if (g_ParcellationFile2Button->value())
		g_ParcellationFile2Disp->activate();
	else
		g_ParcellationFile2Disp->deactivate();
}

void AutoSegGUIControls::ParcellationFile3ButtonChecked()
{
	if (g_ParcellationFile3Button->value())
		g_ParcellationFile3Disp->activate();
	else
		g_ParcellationFile3Disp->deactivate();
}

void AutoSegGUIControls::ComputeVolumeButtonChecked()
{
	if (g_ComputeVolumeButton->value())
		m_Computation.SetComputeVolume(1);
	else
		m_Computation.SetComputeVolume(0);    
}

void AutoSegGUIControls::ComputeCorticalThicknessButtonChecked()
{
	if (g_ComputeCorticalThicknessButton->value())
		m_Computation.SetComputeCorticalThickness(1);
	else
		m_Computation.SetComputeCorticalThickness(0);    
}

void AutoSegGUIControls::RecomputeButtonChecked()
{
	if (g_RecomputeButton->value())
		m_Computation.SetRecompute(1);
	else
		m_Computation.SetRecompute(0);    
}

void AutoSegGUIControls::UseCondorButtonChecked()
{
	m_Computation.SetUseCondor(g_UseCondorButton->value());
}

void AutoSegGUIControls::ShowDisplayButtonPressed()
{
	m_Computation.ShowDisplay();
}

void AutoSegGUIControls::AtlasSpaceButtonChecked()
{
	
	if (g_AtlasSpaceButton->value())
	{
		m_Computation.SetAtlasSpaceImage(1);
		m_Computation.SetSkullStrippedImage(0);
		m_Computation.SetBiasCorrectedImage(0);
	}
	else
		m_Computation.SetAtlasSpaceImage(0);    
}

void AutoSegGUIControls::BiasCorrectedButtonChecked()
{
	
	if (g_BiasCorrectedButton->value())
	{
		m_Computation.SetBiasCorrectedImage(1);
		m_Computation.SetSkullStrippedImage(0);
		m_Computation.SetAtlasSpaceImage(0);
	}
	else
		m_Computation.SetBiasCorrectedImage(0);    
}

void AutoSegGUIControls::StrippedButtonChecked()
{
	
	if (g_StrippedButton->value())
	{
		m_Computation.SetSkullStrippedImage(1);
		m_Computation.SetBiasCorrectedImage(0);
		m_Computation.SetAtlasSpaceImage(0);
	}
	else
		m_Computation.SetSkullStrippedImage(0);    
}

void AutoSegGUIControls::RigidTransformationButtonChecked()
{
	
	if (g_RigidTransformationButton->value())
	{
		m_Computation.SetRigidTransformation(1);
		m_Computation.SetAffineTransformation(0);
		m_Computation.SetBsplineTransformation(0);
	}
	else
		m_Computation.SetRigidTransformation(0);    
}

void AutoSegGUIControls::AffineTransformationButtonChecked()
{
	if (g_AffineTransformationButton->value())
	{
		m_Computation.SetAffineTransformation(1);
		m_Computation.SetRigidTransformation(0);
		m_Computation.SetBsplineTransformation(0);
	}
	else
		m_Computation.SetAffineTransformation(0);    
}

void AutoSegGUIControls::BsplineTransformationButtonChecked()
{
	if (g_BsplineTransformationButton->value())
	{
		m_Computation.SetBsplineTransformation(1);
		m_Computation.SetRigidTransformation(0);
		m_Computation.SetAffineTransformation(0);
	}
	else
		m_Computation.SetBsplineTransformation(0);    
}

void AutoSegGUIControls::ShowMRMLSceneButtonPressed()
{
	if (std::strlen(g_ProcessDataDirectoryDisp->value()) == 0)
		fl_message("Please, set the process data directory...");
	else
	{
		if (fl_choice("Do you want to run Slicer3 with the Mrml scene already loaded?", "No", "Yes", NULL))
		{
			std::string pathSlicer;
			std::string pathSlicerString;
			pathSlicerString= itksys::SystemTools::FindProgram("Slicer3");
			//if path not found
			if(pathSlicerString.empty()==true)
			{
				Fl_File_Chooser fc(".","*",Fl_File_Chooser::SINGLE,"Select the folder where Slicer3* is saved");	
				fc.show();
				while(fc.shown())
					Fl::wait();	
				if(fc.count())
					pathSlicer=fc.value();
			}
			else
			{
				//if the Slicer found is in /Slicer/bin/
				std::string key ("bin/Slicer3");
				size_t found;
				found=pathSlicerString.rfind(key);
				if (found!=std::string::npos)
				pathSlicerString.replace (found,key.length(),"Slicer3");
	
				pathSlicer = pathSlicerString.c_str() ;
			}
			m_Computation.ExecuteSlicer3withScene(pathSlicer);
		}
	}
}

void AutoSegGUIControls::StopAutoSeg()
{
	if (m_Computation.GetIsAutoSegInProcess())
		m_Computation.StopBatchMake();
}

void AutoSegGUIControls::SharpAtlasButtonToggled()
{
	bool IsDefaultSharpAtlasParameterFile;

	m_Computation.SetTissueAtlas("Sharp");
	g_SharpAtlasButton->set();
	g_FuzzyAtlasButton->clear();
	IsDefaultSharpAtlasParameterFile = LoadParameterFile(GetDefaultSharpAtlasParameterFile(),advancedParameters); 
	if (!IsDefaultSharpAtlasParameterFile)
		InitializeParameters(0);
}

void AutoSegGUIControls::FuzzyAtlasButtonToggled()
{
	bool IsDefaultFuzzyAtlasParametersFile;

	m_Computation.SetTissueAtlas("Fuzzy");
	g_SharpAtlasButton->clear();
	g_FuzzyAtlasButton->set();
	IsDefaultFuzzyAtlasParametersFile = LoadParameterFile(GetDefaultFuzzyAtlasParameterFile(),advancedParameters);
	if (!IsDefaultFuzzyAtlasParametersFile)
		InitializeParameters(1);
}

// Compute Automatic Segmentation
void AutoSegGUIControls::ComputeGUI()
{
	int ComputeStudy = 1;

	if (m_Computation.GetIsAutoSegInProcess())
		fl_message("Automatic Segmentation already in process...");
	
	else
	{
		UpdateParameters();
		if (!CheckInputAutoSeg())
		{
			m_Computation.DesallocateDataList();
			m_Computation.DesallocateAuxDataList();
			InitializeData();
			InitializeAuxData();
			m_Computation.SetSubcorticalStructureSegmentation(m_IsSubcorticalStructureSegmentation);
			m_Computation.SetGenericROISegmentation(m_IsGenericROISegmentation);
			m_Computation.SetParcellationMapSegmentation(m_IsParcellationMapSegmentation);
			if (CheckStudy())
			{
				if (g_RecomputeButton->value())
					ComputeStudy = fl_choice("A study already exists. Do you really want to recompute your dataset (and delete current results)?", "No", "Yes", NULL);
				else
					ComputeStudy = fl_choice("A study already exists. Do you really want to compute this study with this set of parameters?", "No", "Yes", NULL);
			}
			if (ComputeStudy)
			{
	      
				m_Computation.SetIsAutoSegInProcess(true);   
				while (m_Computation.GetIsAutoSegInProcess())
				{
					m_Computation.Computation();        
					Fl::check();
				}
			}
		}
	}
}

int AutoSegGUIControls::CheckStudy()
{
	itksys::Glob glob;

	std::vector<std::string> Studies;
	std::string Expression = g_ProcessDataDirectoryDisp->value();
	Expression.insert(Expression.size(),"/AutoSeg_Parameters.txt");
	glob.FindFiles(Expression);
	Studies = glob.GetFiles();  
	return Studies.size();
}

void AutoSegGUIControls::InitializeData()
{
	int Line;

	if (g_DataBrowser->size() >= 2)
	{
		m_Computation.SetNbData(g_DataBrowser->size()-1);
		m_Computation.AllocateDataList();
      
		for (Line = 2; Line <= g_DataBrowser->size(); Line++)
			m_Computation.SetDataList(g_DataBrowser->text(Line), Line-2); 
	}
}

void AutoSegGUIControls::InitializeAuxData()
{
	int Line;
	if (g_AuxDataBrowser->size() >= 2)
	{
		m_Computation.SetNbAuxData(g_AuxDataBrowser->size()-1);
		m_Computation.AllocateAuxDataList();
		for (Line = 2; Line <= g_AuxDataBrowser->size(); Line++)
			m_Computation.SetAuxDataList(g_AuxDataBrowser->text(Line), Line-2);
	}
}


void AutoSegGUIControls::SetSubcorticalStructures()
{
	m_IsSubcorticalStructureSegmentation = 0;

	if (g_AmygdalaLeftButton->value() == 1)
	{
		if (std::strlen(g_AmygdalaLeftDisp->value()) != 0)
		{
			m_Computation.SetAmygdalaLeft(g_AmygdalaLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Amygdala Left...");
	}
	else
		m_Computation.SetAmygdalaLeft("");

	if (g_AmygdalaRightButton->value() == 1)
	{
		if (std::strlen(g_AmygdalaRightDisp->value()) != 0)
		{
			m_Computation.SetAmygdalaRight(g_AmygdalaRightDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Amygdala Right...");
	}
	else
		m_Computation.SetAmygdalaRight("");
  
	if (g_CaudateLeftButton->value() == 1)
	{
		if (std::strlen(g_CaudateLeftDisp->value()) != 0)
		{
			m_Computation.SetCaudateLeft(g_CaudateLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Caudate Left...");
	}
	else
		m_Computation.SetCaudateLeft("");

	if (g_CaudateRightButton->value() == 1)
	{
		if (std::strlen(g_CaudateRightDisp->value()) != 0)
		{
			m_Computation.SetCaudateRight(g_CaudateRightDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Caudate Right...");
	}
	else
		m_Computation.SetCaudateRight("");

	if (g_HippocampusLeftButton->value() == 1)
	{
		if (std::strlen(g_HippocampusLeftDisp->value()) != 0)
		{
			m_Computation.SetHippocampusLeft(g_HippocampusLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Hippocampus Left...");
	}
	else
		m_Computation.SetHippocampusLeft("");

	if (g_HippocampusRightButton->value() == 1)
	{
		if (std::strlen(g_HippocampusRightDisp->value()) != 0)
		{
			m_Computation.SetHippocampusRight(g_HippocampusRightDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Hippocampus Right...");
	}
	else
		m_Computation.SetHippocampusRight("");

	if (g_PallidusLeftButton->value() == 1)
	{
		if (std::strlen(g_PallidusLeftDisp->value()) != 0)
		{
			m_Computation.SetPallidusLeft(g_PallidusLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Pallidus Left...");
	}
	else
		m_Computation.SetPallidusLeft("");

	if (g_PallidusRightButton->value() == 1)
	{
		if (std::strlen(g_PallidusRightDisp->value()) != 0)
		{
			m_Computation.SetPallidusRight(g_PallidusRightDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Pallidus Right...");
	}
	else
		m_Computation.SetPallidusRight("");

	if (g_PutamenLeftButton->value() == 1)
	{
		if (std::strlen(g_PutamenLeftDisp->value()) != 0)
		{
			m_Computation.SetPutamenLeft(g_PutamenLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Putamen Left...");
	}
	else
		m_Computation.SetPutamenLeft("");

	if (g_PutamenRightButton->value() == 1)
	{
		if (std::strlen(g_PutamenRightDisp->value()) != 0)
		{
			m_Computation.SetPutamenRight(g_PutamenRightDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Putamen Right...");
	}
	else
		m_Computation.SetPutamenRight("");

	if (g_LateralVentricleLeftButton->value() == 1)
	{
		if (std::strlen(g_LateralVentricleLeftDisp->value()) != 0)
		{
			m_Computation.SetLateralVentricleLeft(g_LateralVentricleLeftDisp->value());
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Lateral Ventricle Left...");
	}
	else
		m_Computation.SetLateralVentricleLeft("");

	if (g_LateralVentricleRightButton->value() == 1)
	{
		if (std::strlen(g_LateralVentricleRightDisp->value()) != 0)
		{
			m_Computation.SetLateralVentricleRight(g_LateralVentricleRightDisp->value()); 
			m_IsSubcorticalStructureSegmentation = 1;
		}
		else
			fl_message("Please, set the Lateral Ventricle Right...");
	}
	else
		m_Computation.SetLateralVentricleRight("");  
}

void AutoSegGUIControls::SetGenericROIMaps()
{
	m_IsGenericROISegmentation = 0;

	if (g_ROIFile1Button->value() == 1)
	{
		if (std::strlen(g_ROIFile1Disp->value()) != 0)
		{
			m_Computation.SetROIFile1(g_ROIFile1Disp->value());
			m_IsGenericROISegmentation = 1;
		}
		else
			fl_message("Please, set the Generic ROI File 1...");
	}
	else
		m_Computation.SetROIFile1("");
	if (g_ROIFile2Button->value() == 1)
	{
		if (std::strlen(g_ROIFile2Disp->value()) != 0)
		{
			m_Computation.SetROIFile2(g_ROIFile2Disp->value());
			m_IsGenericROISegmentation = 1;
		}
		else
			fl_message("Please, set the Generic ROI File 2...");
	}
	else
		m_Computation.SetROIFile2("");
	if (g_ROIFile3Button->value() == 1) 
	{
		if (std::strlen(g_ROIFile3Disp->value()) != 0)
		{
			m_Computation.SetROIFile3(g_ROIFile3Disp->value());
			m_IsGenericROISegmentation = 1;
		}
		else
			fl_message("Please, set the Generic ROI File 3...");
	}
	else
		m_Computation.SetROIFile3("");
	if (g_ROIFile4Button->value() == 1)
	{
		if (std::strlen(g_ROIFile4Disp->value()) != 0)
		{
			m_Computation.SetROIFile4(g_ROIFile4Disp->value());
			m_IsGenericROISegmentation = 1;
		}
		else
			fl_message("Please, set the Generic ROI File 4...");
	}
	else
		m_Computation.SetROIFile4("");
	if (g_ROIFile5Button->value() == 1)
	{
		if (std::strlen(g_ROIFile5Disp->value()) != 0)
		{
			m_Computation.SetROIFile5(g_ROIFile5Disp->value());
			m_IsGenericROISegmentation = 1;
		}
		else
			fl_message("Please, set the Generic ROI File 5...");
	}
	else
		m_Computation.SetROIFile5("");  
}

void AutoSegGUIControls::SetParcellationMap()
{
	m_IsParcellationMapSegmentation = 0;

	if (g_ParcellationFile1Button->value() == 1)
	{
		if (std::strlen(g_ParcellationFile1Disp->value()) != 0)
		{
			m_Computation.SetParcellationFile1(g_ParcellationFile1Disp->value());
			m_IsParcellationMapSegmentation = 1;
		}
		else
			fl_message("Please, set the Parcellation File 1...");
	}
	else
		m_Computation.SetParcellationFile1("");
	if (g_ParcellationFile2Button->value() == 1)
	{
		if (std::strlen(g_ParcellationFile2Disp->value()) != 0)
		{
			m_Computation.SetParcellationFile2(g_ParcellationFile2Disp->value());
			m_IsParcellationMapSegmentation = 1;
		}
		else
			fl_message("Please, set the Parcellation File 2...");
	}
	else
		m_Computation.SetParcellationFile2("");
	if (g_ParcellationFile3Button->value() == 1)
	{
		if (std::strlen(g_ParcellationFile3Disp->value()) != 0)
		{
			m_Computation.SetParcellationFile3(g_ParcellationFile3Disp->value());
			m_IsParcellationMapSegmentation = 1;
		}
		else
			fl_message("Please, set the Parcellation File 3...");
	}
	else
		m_Computation.SetParcellationFile3("");  
}

void AutoSegGUIControls::SetGridTemplateParameters()
{
	if (g_GridTemplateAtlasButton->value())
		m_Computation.SetROIAtlasGridTemplate(true);
	else
	{
		m_Computation.SetROIAtlasGridTemplate(false);
		m_Computation.SetGridTemplateSizeX((int)g_GridTemplateSizeX->value());
		m_Computation.SetGridTemplateSizeY((int)g_GridTemplateSizeY->value());
		m_Computation.SetGridTemplateSizeZ((int)g_GridTemplateSizeZ->value());
		m_Computation.SetGridTemplateSpacingX((float)g_GridTemplateSpacingX->value());
		m_Computation.SetGridTemplateSpacingY((float)g_GridTemplateSpacingY->value());
		m_Computation.SetGridTemplateSpacingZ((float)g_GridTemplateSpacingZ->value());
	}
}

void AutoSegGUIControls::SetIntensityRescalingMethod()
{
	if (g_HistogramQuantileButton->value())
		m_Computation.SetIntensityRescalingMethod(1);
	else
		m_Computation.SetIntensityRescalingMethod(2);
}

//Check Input Automatic Segmentation Computation
bool AutoSegGUIControls::CheckInputAutoSeg()
{
	bool Warning = false;

	if (std::strlen(g_ProcessDataDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the process data directory (Step 1)...");
		Warning = true;
	}
	else if (std::strlen(g_DataAutoSegDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the output directory for each data (Step 3)...");
		Warning = true;
	}
	else if (g_DataBrowser->size() < 2)
	{
		fl_message("Please, set the data to be computed (Step 4)...");
		Warning = true;
	}
	else if (std::strlen(g_CommonCoordinateImageDisp->value()) == 0)
	{
		fl_message("Please, set the common coordinate image...");
		Warning = true;
	}   
	else if (std::strlen(g_TissueSegmentationAtlasDirectoryDisp->value()) == 0)
	{
		fl_message("Please, set the tissue segmentation...");
		Warning = true;
	}  
	else if (std::strlen(g_ROIAtlasFileDisp->value()) == 0)
	{
		fl_message("Please, set the ROI atlas file...");
		Warning = true;
	}   
	else if ( (m_IsSubcorticalStructureSegmentation == 0) && (m_IsGenericROISegmentation == 0) && (m_IsParcellationMapSegmentation == 0) )
	{
		fl_message("Please, set a file to be segmented...");
		Warning = true;
	}  
	else if ( (g_GridTemplateManualButton->value() == 1) && ( (g_GridTemplateSizeX->value() == 0)||(g_GridTemplateSizeY->value() == 0)||(g_GridTemplateSizeZ->value() == 0)||(g_GridTemplateSpacingX->value() == 0)||(g_GridTemplateSpacingY->value() == 0)||(g_GridTemplateSpacingZ->value() == 0)) )
	{
		fl_message("Please, set GridTemplate parameters with values different than 0...");
		Warning = true;
	}
	else if (g_Aux1Button->value() != 0)
	{
		if ((g_AtlasSpaceButton->value() == 0) && (g_StrippedButton->value() == 0) && (g_BiasCorrectedButton->value() == 0))
		{
			fl_message("Please, set the process step of the source data...");
			Warning = true;
		}
		else if ((g_RigidTransformationButton->value() == 0) && (g_AffineTransformationButton->value() == 0) && (g_BsplineTransformationButton->value() == 0))
		{
			fl_message("Please, set the transformation...");
			Warning = true;
		}
		else if ((g_Aux1Button->value() != 0) && (std::strlen(g_Aux1Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 1 directory name...");
			Warning = true;
		}
		else if ((g_Aux2Button->value() != 0) && (std::strlen(g_Aux2Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 2 directory name...");
			Warning = true;
		}
		else if ((g_Aux3Button->value() != 0) && (std::strlen(g_Aux3Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 3 directory name...");
			Warning = true;
		}
		else if ((g_Aux4Button->value() != 0) && (std::strlen(g_Aux4Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 4 directory name...");
			Warning = true;
		}
		else if ((g_Aux5Button->value() != 0) && (std::strlen(g_Aux5Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 5 directory name...");
			Warning = true;
		}
		else if ((g_Aux6Button->value() != 0) && (std::strlen(g_Aux6Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 6 directory name...");
			Warning = true;
		}
		else if ((g_Aux7Button->value() != 0) && (std::strlen(g_Aux7Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 7 directory name...");
			Warning = true;
		}
		else if ((g_Aux8Button->value() != 0) && (std::strlen(g_Aux8Disp->value()) == 0))
		{
			fl_message("Please, set the auxiliairy 8 directory name...");
			Warning = true;
		}		
		else if (g_AuxDataBrowser->size() < 2)
		{
			fl_message("Please, set the auxiliairy data to be computed...");
			Warning = true;
		}
	}
	return Warning;
}

void AutoSegGUIControls::ABCButtonToggled()
{
	g_ABCButton->set();

  // AdvancedParameters
	g_FilterMethodChoice->activate();
	g_FilterMethodChoice->value(1);
	g_FilterIterations->value(10);
	g_FluidAtlasWarpButton->clear();
	g_FluidAtlasAffineButton->set();
	g_FluidAtlasFATWButton->clear();
	g_FluidAtlasWarpGroup->activate();
	g_FluidAtlasWarpIterations->value(10);
	g_FluidAtlasWarpMaxStep->value(0.5);
	g_AtlasLinearMappingChoice->value(0);
	g_ImageLinearMappingChoice->value(0);

	m_Computation.SetEMSoftware("ABC");
	m_Computation.SetFilterIterations((int)g_FilterIterations->value());
	m_Computation.SetFilterMethod("Curvature flow");
	m_Computation.SetFluidAtlasWarp(0);
	m_Computation.SetFluidAtlasAffine(1);
	m_Computation.SetFluidAtlasFATW(0);
	m_Computation.SetFluidAtlasWarpIterations((int)g_FluidAtlasWarpIterations->value());
	m_Computation.SetFluidAtlasWarpMaxStep((float)g_FluidAtlasWarpMaxStep->value());
	m_Computation.SetAtlasLinearMapping("affine");
	m_Computation.SetImageLinearMapping("id");
}
void AutoSegGUIControls::LoopButtonChecked()
{
	if (g_LoopButton->value())
	{
		g_AtlasLoopGroup->activate();
		g_LoopIteration->activate();
		g_FluidAtlasFATWButton->activate();
		m_Computation.SetLoop(1);
	}
	else
	{
		g_AtlasLoopGroup->deactivate();
		g_LoopIteration->deactivate();
		g_FluidAtlasFATWButton->deactivate();
		m_Computation.SetLoop(0);
	}
}

void AutoSegGUIControls::SetLoopIterationGUI()
{
	m_Computation.SetLoopIteration((int)g_LoopIteration->value());
}

void AutoSegGUIControls::SetFilterIterationsGUI()
{
	m_Computation.SetFilterIterations((int)g_FilterIterations->value());
}

void AutoSegGUIControls::SetFilterTimeStepGUI()
{
	m_Computation.SetFilterTimeStep((float)g_FilterTimeStep->value());
}

void AutoSegGUIControls::SetFilterMethodChoiceGUI()
{
	if (g_FilterMethodChoice->value() == 1)
		m_Computation.SetFilterMethod("Curvature flow");
	else
		m_Computation.SetFilterMethod("Grad aniso diffusion");
}

void AutoSegGUIControls::SetMaxBiasDegreeGUI()
{
	m_Computation.SetMaxBiasDegree((int)g_MaxBiasDegree->value());
}

void AutoSegGUIControls::SetPrior1GUI()
{
	m_Computation.SetPrior1((float)g_Prior1->value());
}

void AutoSegGUIControls::SetPrior2GUI()
{ 
	m_Computation.SetPrior2((float)g_Prior2->value());
}

void AutoSegGUIControls::SetPrior3GUI()
{
	m_Computation.SetPrior3((float)g_Prior3->value());
}

void AutoSegGUIControls::SetPrior4GUI()
{
	m_Computation.SetPrior4((float)g_Prior4->value());
}

void AutoSegGUIControls::FluidAtlasWarpButtonChecked()
{
	g_FluidAtlasWarpButton->set();
	g_FluidAtlasAffineButton->clear();
	g_FluidAtlasFATWButton->clear();
	m_Computation.SetFluidAtlasWarp(g_FluidAtlasWarpButton->value());
	m_Computation.SetFluidAtlasAffine(g_FluidAtlasAffineButton->value());
	m_Computation.SetFluidAtlasFATW(g_FluidAtlasFATWButton->value());
}

void AutoSegGUIControls::FluidAtlasAffineButtonChecked()
{
	g_FluidAtlasWarpButton->clear();
	g_FluidAtlasAffineButton->set();
	g_FluidAtlasFATWButton->clear();
	m_Computation.SetFluidAtlasWarp(g_FluidAtlasWarpButton->value());
	m_Computation.SetFluidAtlasAffine(g_FluidAtlasAffineButton->value());
	m_Computation.SetFluidAtlasFATW(g_FluidAtlasFATWButton->value());
}

void AutoSegGUIControls::FluidAtlasFATWButtonChecked()
{
	g_FluidAtlasWarpButton->clear();
	g_FluidAtlasAffineButton->clear();
	g_FluidAtlasFATWButton->set();
	m_Computation.SetFluidAtlasWarp(g_FluidAtlasWarpButton->value());
	m_Computation.SetFluidAtlasAffine(g_FluidAtlasAffineButton->value());
	m_Computation.SetFluidAtlasFATW(g_FluidAtlasFATWButton->value());
}

void AutoSegGUIControls::SetFluidAtlasWarpIterationsGUI()
{
	m_Computation.SetFluidAtlasWarpIterations((int)g_FluidAtlasWarpIterations->value());
}

void AutoSegGUIControls::SetFluidAtlasWarpMaxStepGUI()
{
  m_Computation.SetFluidAtlasWarpMaxStep((float)g_FluidAtlasWarpMaxStep->value());
}

void AutoSegGUIControls::SetAtlasLinearMappingChoiceGUI()
{
	if (g_AtlasLinearMappingChoice->value() == 0)
		m_Computation.SetAtlasLinearMapping("affine");
	else if(g_AtlasLinearMappingChoice->value() == 1)
		m_Computation.SetAtlasLinearMapping("id");
	else
		m_Computation.SetAtlasLinearMapping("rigid");
}

void AutoSegGUIControls::SetImageLinearMappingChoiceGUI()
{
	if (g_ImageLinearMappingChoice->value() == 0)
		m_Computation.SetImageLinearMapping("id");
	else if(g_ImageLinearMappingChoice->value() == 1)
		m_Computation.SetImageLinearMapping("rigid");
	else
		m_Computation.SetImageLinearMapping("affine");
}

void AutoSegGUIControls::RigidRegistrationButtonChecked()
{
	if (g_RigidRegistrationButton->value())
	{
		g_RigidRegistrationGroup->activate();
		m_Computation.SetRigidRegistration(1);
	}
	else
	{
		g_RigidRegistrationGroup->deactivate();
		m_Computation.SetRigidRegistration(0);
	}
}

void AutoSegGUIControls::GridTemplateAtlasButtonToggled()
{
	g_GridTemplateAtlasButton->set();
	g_GridTemplateManualButton->clear();
	g_GridParametersGroup->deactivate();
}

void AutoSegGUIControls::GridTemplateManualButtonToggled()
{
	g_GridTemplateManualButton->set();
	g_GridTemplateAtlasButton->clear();
	g_GridParametersGroup->activate();
}

void AutoSegGUIControls::ClassicWarpingButtonToggled()
{
	g_ClassicWarpingButton->set();
	g_CoarseToFineWarpingButton->clear();
	g_BRAINSDemonWarpButton->clear();

	g_Alpha->activate();
	g_Beta->activate();
	g_Gamma->activate();
	g_MaxPerturbation->activate();
	g_Scale1NbIterations->activate();
	g_NumBasis->value(0.01);
	g_NumBasis->activate();
	g_Scale4NbIterations->deactivate();
	g_Scale2NbIterations->deactivate();
	g_Scale1NbIterations->value(100);
	g_BRAINSDemonWarp->deactivate();
  
	m_Computation.SetClassicWarpingMethod(1);
	m_Computation.SetBRAINSDemonWarpMethod(0);
	m_Computation.SetNumBasis((float)g_NumBasis->value());
	m_Computation.SetScale1NbIterations((int)g_Scale1NbIterations->value());
}

void AutoSegGUIControls::CoarseToFineWarpingButtonToggled()
{
	g_ClassicWarpingButton->clear();
	g_CoarseToFineWarpingButton->set();
	g_BRAINSDemonWarpButton->clear();

	g_Alpha->activate();
	g_Beta->activate();
	g_Gamma->activate();
	g_MaxPerturbation->activate();
	g_Scale1NbIterations->activate();
	g_NumBasis->value(2000);
	g_NumBasis->deactivate();
	g_Scale4NbIterations->activate();
	g_Scale2NbIterations->activate();
	g_Scale4NbIterations->value(50);
	g_Scale2NbIterations->value(25);
	g_Scale1NbIterations->value(100);
	g_BRAINSDemonWarp->deactivate();
  
	m_Computation.SetClassicWarpingMethod(0);
	m_Computation.SetBRAINSDemonWarpMethod(0);
	m_Computation.SetNumBasis((float)g_NumBasis->value());
	m_Computation.SetScale4NbIterations((int)g_Scale4NbIterations->value());
	m_Computation.SetScale2NbIterations((int)g_Scale2NbIterations->value());
	m_Computation.SetScale1NbIterations((int)g_Scale1NbIterations->value());
}

void AutoSegGUIControls::BRAINSDemonWarpToggled()
{
	g_ClassicWarpingButton->clear();
	g_CoarseToFineWarpingButton->clear();
	g_BRAINSDemonWarpButton->set();

	g_NumBasis->deactivate();
	g_Scale4NbIterations->deactivate();
	g_Scale2NbIterations->deactivate();
	g_Scale1NbIterations->deactivate();
	g_Alpha->deactivate();
	g_Beta->deactivate();
	g_Gamma->deactivate();
	g_MaxPerturbation->deactivate();
	g_BRAINSDemonWarp->activate();
  
	m_Computation.SetBRAINSDemonWarpMethod(1);
	m_Computation.SetClassicWarpingMethod(0);
}

void AutoSegGUIControls::SetAlphaGUI()
{
	m_Computation.SetAlpha((float)g_Alpha->value());
}

void AutoSegGUIControls::SetBetaGUI()
{
	m_Computation.SetBeta((float)g_Beta->value());
}

void AutoSegGUIControls::SetGammaGUI()
{
	m_Computation.SetGamma((float)g_Gamma->value());
}

void AutoSegGUIControls::SetMaxPerturbationGUI()
{
	m_Computation.SetMaxPerturbation((float)g_MaxPerturbation->value());
}

void AutoSegGUIControls::SetScale4NbIterationsGUI()
{
	m_Computation.SetScale4NbIterations((int)g_Scale4NbIterations->value());
}

void AutoSegGUIControls::SetScale2NbIterationsGUI()
{
	m_Computation.SetScale2NbIterations((int)g_Scale2NbIterations->value());
}

void AutoSegGUIControls::SetScale1NbIterationsGUI()
{
	m_Computation.SetScale1NbIterations((int)g_Scale1NbIterations->value());
}

void AutoSegGUIControls::SetNumBasisGUI()
{
	m_Computation.SetNumBasis((float)g_NumBasis->value());
}

void AutoSegGUIControls::SetPyramidLevelsGUI()
{
	m_Computation.SetPyramidLevels((int)g_PyramidLevels->value());
}

void AutoSegGUIControls::SetMovingShrinkFactorsGUI()
{
	m_Computation.SetMovingShrinkFactors((const char *)g_MovingShrinkFactors->value());
}

void AutoSegGUIControls::SetFixedShrinkFactorsGUI()
{
	m_Computation.SetFixedShrinkFactors((const char *)g_FixedShrinkFactors->value());
}

void AutoSegGUIControls::SetIterationCountPyramidLevelsGUI()
{
	m_Computation.SetIterationCountPyramidLevels((const char *)g_IterationCountPyramidLevels->value());
}

void AutoSegGUIControls::SetDeformationFieldSmoothingSigmaGUI()
{
	m_Computation.SetDeformationFieldSmoothingSigma((float)g_DeformationFieldSmoothingSigma->value());
}

void AutoSegGUIControls::SetRegistrationFilterTypeGUI()
{
	if (g_RegistrationFilterType->value() == 0)
		m_Computation.SetRegistrationFilterType("Demons");
	else if(g_RegistrationFilterType->value() == 1)
		m_Computation.SetRegistrationFilterType("FastSymmetricForces");
	else if(g_RegistrationFilterType->value() == 2)
		m_Computation.SetRegistrationFilterType("Diffeomorphic");
	else if(g_RegistrationFilterType->value() == 3)
		m_Computation.SetRegistrationFilterType("LogDemons");
	else if(g_RegistrationFilterType->value() == 4)
		m_Computation.SetRegistrationFilterType("SymmetricLogDemons");
}

void AutoSegGUIControls::UseDefaultEMSAdvancedParametersGUI()
{
	if (g_SharpAtlasButton->value())
		LoadParameterFile(GetDefaultSharpAtlasParameterFile(),tissueSeg);
	else
		LoadParameterFile(GetDefaultFuzzyAtlasParameterFile(),tissueSeg);
}

void AutoSegGUIControls::UseDefaultWarpingAdvancedParametersGUI()
{
	if (g_SharpAtlasButton->value())
		LoadParameterFile(GetDefaultSharpAtlasParameterFile(),warping);
	else
		LoadParameterFile(GetDefaultFuzzyAtlasParameterFile(),warping);
}

void AutoSegGUIControls::UseDefaultN4AdvancedParametersGUI()
{
	if (g_SharpAtlasButton->value())
		LoadParameterFile(GetDefaultSharpAtlasParameterFile(),N4biasFieldCorrection);
	else
		LoadParameterFile(GetDefaultFuzzyAtlasParameterFile(),N4biasFieldCorrection);
}

void AutoSegGUIControls::N4ITKBiasFieldCorrectionButtonChecked()
{
	if (g_N4ITKBiasFieldCorrectionButton->value())
	{
		g_N4ParametersGroup->activate();
		g_N4AdvancedParametersGroup->activate();
		m_Computation.SetN4ITKBiasFieldCorrection(1);
	}
	else
	{
		g_N4ParametersGroup->deactivate();
		g_N4AdvancedParametersGroup->deactivate();
		m_Computation.SetN4ITKBiasFieldCorrection(0);
	}
}

void AutoSegGUIControls::SetNbOfIterationsGUI()
{
	m_Computation.SetNbOfIterations((const char *)g_NbOfIterations->value());
}

void AutoSegGUIControls::SetBSplineGridResolutionsGUI()
{
	m_Computation.SetBSplineGridResolutions((const char *)g_BSplineGridResolutions->value());
}

void AutoSegGUIControls::SetConvergenceThresholdGUI()
{
	m_Computation.SetConvergenceThreshold((float)g_ConvergenceThreshold->value());
}

void AutoSegGUIControls::SetSplineDistanceGUI()
{
	m_Computation.SetSplineDistance((float)g_SplineDistance->value());
}

void AutoSegGUIControls::SetShrinkFactorGUI()
{
	m_Computation.SetShrinkFactor((int)g_ShrinkFactor->value());
}

void AutoSegGUIControls::SetBSplineOrderGUI()
{
	m_Computation.SetBSplineOrder((int)g_BSplineOrder->value());
}

void AutoSegGUIControls::SetBSplineAlphaGUI()
{
	m_Computation.SetBSplineAlpha((float)g_BSplineAlpha->value());
}

void AutoSegGUIControls::SetBSplineBetaGUI()
{
	m_Computation.SetBSplineBeta((float)g_BSplineBeta->value());
}

void AutoSegGUIControls::SetHistogramSharpeningGUI()
{
	m_Computation.SetHistogramSharpening((const char *)g_HistogramSharpening->value());
}

void AutoSegGUIControls::DeleteVesselsButtonChecked()
{
	if (g_DeleteVesselsButton->value())
		m_Computation.SetDeleteVessels(1);
	else
		m_Computation.SetDeleteVessels(0);    
}

void AutoSegGUIControls::HistogramQuantileButtonToggled()
{
	g_HistogramQuantileButton->set();
	g_TissueMeanMatchButton->clear();
}

void AutoSegGUIControls::TissueMeanMatchButtonToggled()
{
	g_TissueMeanMatchButton->set();
	g_HistogramQuantileButton->clear();
}

void AutoSegGUIControls::SetPointSpacingGUI()
{
  m_Computation.SetPointSpacing((int)g_PointSpacingDisp->value());
}

// Initialize Parameters
// _TissueAtlas = 0 (default) -> Sharp Atlas
// _TissueAtlas = 1 -> Fuzzy Atlas
void AutoSegGUIControls::InitializeParameters(bool _TissueAtlas)
{
  // Age Group Parameter
	if (!_TissueAtlas)
	{
		g_SharpAtlasButton->set();
		g_FuzzyAtlasButton->clear();
		m_Computation.SetTissueAtlas("Sharp");
	}
	else
	{
		g_SharpAtlasButton->clear();
		g_FuzzyAtlasButton->set();      
		m_Computation.SetTissueAtlas("Fuzzy");
	}

  // Atlases Parameters
	g_TissueSegmentationAtlasDirectoryDisp->value("/tools/atlas/BrainsegAtlas/adult-atlas-asym-T1-RAI/");
	g_TissueSegmentationAtlasT1Button->set();
	g_TissueSegmentationAtlasT2Button->clear();  
	m_Computation.SetTissueSegmentationAtlasDirectory(g_TissueSegmentationAtlasDirectoryDisp->value());
	m_Computation.SetTissueSegmentationAtlasType("T1");
    
	g_CommonCoordinateImageDisp->value("/tools/atlas/BrainsegAtlas/adult-atlas-asym-T1-RAI/template.nrrd");
	g_CommonCoordinateImageT1Button->set();
	g_CommonCoordinateImageT2Button->clear();
	m_Computation.SetCommonCoordinateImage(g_CommonCoordinateImageDisp->value());
	m_Computation.SetCommonCoordinateImageType("T1");

	g_ROIAtlasFileDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/template.nrrd");
	m_Computation.SetROIAtlasFile(g_ROIAtlasFileDisp->value());

  // Probabilistic Subcortical Structures Parameters
	g_AllStructuresButton->set();
	AllStructuresButtonChecked();
	g_AmygdalaLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/amygdalaLeft.nrrd");
	g_AmygdalaRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/amygdalaRight.nrrd");
	g_CaudateLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/caudateLeft.nrrd");
	g_CaudateRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/caudateRight.nrrd");
	g_HippocampusLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/hippocampusLeft.nrrd");
	g_HippocampusRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/hippocampusRight.nrrd");
	g_PallidusLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/pallidusLeft.nrrd");
	g_PallidusRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/pallidusRight.nrrd");
	g_PutamenLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/putamenLeft.nrrd");
	g_PutamenRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/putamenRight.nrrd");
	g_LateralVentricleLeftDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/latVentricleLeftMask.nrrd");
	g_LateralVentricleRightDisp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/latVentricleRightMask.nrrd");
	m_Computation.SetAmygdalaLeft(g_AmygdalaLeftDisp->value());
	m_Computation.SetAmygdalaRight(g_AmygdalaRightDisp->value());
	m_Computation.SetCaudateLeft(g_CaudateLeftDisp->value());
	m_Computation.SetCaudateRight(g_CaudateRightDisp->value());
	m_Computation.SetHippocampusLeft(g_HippocampusLeftDisp->value());
	m_Computation.SetHippocampusRight(g_HippocampusRightDisp->value());
	m_Computation.SetPallidusLeft(g_PallidusLeftDisp->value());
	m_Computation.SetPallidusRight(g_PallidusRightDisp->value());
	m_Computation.SetPutamenLeft(g_PutamenLeftDisp->value());
	m_Computation.SetPutamenRight(g_PutamenRightDisp->value());
	m_Computation.SetLateralVentricleLeft(g_LateralVentricleLeftDisp->value());
	m_Computation.SetLateralVentricleRight(g_LateralVentricleRightDisp->value());  

  //Parcellation Map
	g_ParcellationFile1Disp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/Parcellation.nrrd");
	g_ParcellationFile2Disp->value(NULL);
	g_ParcellationFile3Disp->value(NULL);
	g_ParcellationFile1Disp->activate();
	g_ParcellationFile2Disp->deactivate();
	g_ParcellationFile3Disp->deactivate();
	g_ParcellationFile1Button->set();
	g_ParcellationFile2Button->clear();
	g_ParcellationFile2Button->clear();  
	m_Computation.SetParcellationFile1(g_ParcellationFile1Disp->value());
	m_Computation.SetParcellationFile2(g_ParcellationFile2Disp->value());
	m_Computation.SetParcellationFile3(g_ParcellationFile3Disp->value());
  
  //Generic ROI Maps
	g_ROIFile1Disp->value("/tools/atlas/BrainROIAtlas/adultT1_RAI/nrrd_aligned_Brainsegatlas_adult-asym/HuangROIAtlas.nrrd");
	g_ROIFile2Disp->value(NULL);
	g_ROIFile3Disp->value(NULL);
	g_ROIFile4Disp->value(NULL);
	g_ROIFile5Disp->value(NULL);  
	g_ROIFile1Button->set();
	g_ROIFile2Button->clear();
	g_ROIFile3Button->clear();
	g_ROIFile4Button->clear();
	g_ROIFile5Button->clear();
	g_ROIFile1Disp->activate();
	g_ROIFile2Disp->deactivate();
	g_ROIFile3Disp->deactivate();
	g_ROIFile4Disp->deactivate();
	g_ROIFile5Disp->deactivate();
	m_Computation.SetROIFile1(g_ROIFile1Disp->value());
	m_Computation.SetROIFile2(g_ROIFile2Disp->value());
	m_Computation.SetROIFile3(g_ROIFile3Disp->value());
	m_Computation.SetROIFile4(g_ROIFile4Disp->value());
	m_Computation.SetROIFile5(g_ROIFile5Disp->value());
  

  //Tissue Segmentation Parameters
	g_ABCButton->set();
	g_FilterIterations->value(10);
	g_FilterTimeStep->value(0.01);
	g_FilterMethodChoice->activate();
	g_FilterMethodChoice->value(1);
	g_MaxBiasDegree->value(4);
	g_Prior1->value(1.3);
	g_Prior2->value(1.0);
	g_Prior3->value(0.7);
	g_Prior4->value(1.0);
	g_FluidAtlasWarpGroup->activate();
	g_FluidAtlasWarpButton->clear();
	g_FluidAtlasAffineButton->set();
	g_FluidAtlasFATWButton->clear();
	g_FluidAtlasWarpIterations->value(10);
	g_FluidAtlasWarpMaxStep->value(0.5);
	g_AtlasLinearMappingChoice->value(0);
	g_ImageLinearMappingChoice->value(0);
	g_AtlasLoopDisp->value("/tools/atlas/BrainsegAtlas/adult-atlas-asym-stripped-T1-RAI/");
	g_LoopIteration->value(1);

	m_Computation.SetFilterIterations((int)g_FilterIterations->value());
	m_Computation.SetFilterTimeStep((float)g_FilterTimeStep->value());
	m_Computation.SetFilterMethod("Curvature flow");
	m_Computation.SetMaxBiasDegree((int)g_MaxBiasDegree->value());
	m_Computation.SetPrior1((float)g_Prior1->value());
	m_Computation.SetPrior2((float)g_Prior2->value());
	m_Computation.SetPrior3((float)g_Prior3->value());
	m_Computation.SetPrior4((float)g_Prior4->value());
	m_Computation.SetFluidAtlasWarp(0);
	m_Computation.SetFluidAtlasAffine(1);
	m_Computation.SetFluidAtlasFATW(0);
	m_Computation.SetFluidAtlasWarpIterations((int)g_FluidAtlasWarpIterations->value());
	m_Computation.SetFluidAtlasWarpMaxStep((float)g_FluidAtlasWarpMaxStep->value());
	m_Computation.SetAtlasLinearMapping("affine");
	m_Computation.SetImageLinearMapping("id");
	m_Computation.SetAtlasLoop(g_AtlasLoopDisp->value());
	m_Computation.SetLoopIteration((int)g_LoopIteration->value());

  // N4ITKBiasFieldCorrection Parameters
	g_N4ITKBiasFieldCorrectionButton->set();
	g_N4ParametersGroup->activate();
	g_N4AdvancedParametersGroup->activate();
	g_NbOfIterations->value("50,40,30");
	g_BSplineGridResolutions->value("1,1,1");
	g_ConvergenceThreshold->value(0.0001);
	g_SplineDistance->value(0);
	g_ShrinkFactor->value(4);
	g_BSplineOrder->value(3);
	g_BSplineAlpha->value(0);
	g_BSplineBeta->value(0.5);
	g_HistogramSharpening->value("0");
	m_Computation.SetN4ITKBiasFieldCorrection(0);
	m_Computation.SetNbOfIterations ("50,40,30");
	m_Computation.SetBSplineGridResolutions ("1,1,1");
	m_Computation.SetConvergenceThreshold((float)g_ConvergenceThreshold->value());
	m_Computation.SetSplineDistance((int)g_SplineDistance->value());
	m_Computation.SetShrinkFactor((int)g_ShrinkFactor->value());
	m_Computation.SetBSplineOrder((int)g_BSplineOrder->value());
	m_Computation.SetBSplineAlpha((int)g_BSplineAlpha->value());
	m_Computation.SetBSplineBeta((float)g_BSplineBeta->value());
	m_Computation.SetHistogramSharpening("0");

  // Rigid Registration Parameters
	g_RigidRegistrationButton->set();
	g_GridTemplateAtlasButton->set();
	g_GridTemplateManualButton->clear();
	g_GridTemplateSizeX->value(0);
	g_GridTemplateSizeY->value(0);
	g_GridTemplateSizeZ->value(0);
	g_GridTemplateSpacingX->value(0);
	g_GridTemplateSpacingY->value(0);
	g_GridTemplateSpacingZ->value(0);  
	m_Computation.SetRigidRegistration(1);
	m_Computation.SetROIAtlasGridTemplate((bool)g_GridTemplateAtlasButton->value());  
	m_Computation.SetGridTemplateSizeX((int)g_GridTemplateSizeX->value());
	m_Computation.SetGridTemplateSizeY((int)g_GridTemplateSizeY->value());
	m_Computation.SetGridTemplateSizeZ((int)g_GridTemplateSizeZ->value());
	m_Computation.SetGridTemplateSpacingX((float)g_GridTemplateSpacingX->value());
	m_Computation.SetGridTemplateSpacingY((float)g_GridTemplateSpacingY->value());
	m_Computation.SetGridTemplateSpacingZ((float)g_GridTemplateSpacingZ->value());
  
  // Warping Parameters
	g_ClassicWarpingButton->clear();
	g_CoarseToFineWarpingButton->clear();
	g_BRAINSDemonWarpButton->set();
	g_Alpha->value(0.01);
	g_Beta->value(0.01);
	g_Gamma->value(0.001);
	g_MaxPerturbation->value(0.5);
	g_NumBasis->value(0.01);
	g_Alpha->deactivate();
	g_Beta->deactivate();
	g_Gamma->deactivate();
	g_MaxPerturbation->deactivate();
	g_NumBasis->deactivate();
	g_Scale4NbIterations->value(50);
	g_Scale4NbIterations->deactivate();
	g_Scale2NbIterations->value(25);
	g_Scale2NbIterations->deactivate();
	g_Scale1NbIterations->value(100);
	g_Scale1NbIterations->deactivate();
	g_BRAINSDemonWarp->activate();
	g_RegistrationFilterType->value(3);
	g_DeformationFieldSmoothingSigma->value(1.0);
	g_PyramidLevels->value(5);
	g_MovingShrinkFactors->value("16,16,16");
	g_FixedShrinkFactors->value("16,16,16");
	g_IterationCountPyramidLevels->value("300,50,30,20,15");
	m_Computation.SetClassicWarpingMethod(0);
	m_Computation.SetBRAINSDemonWarpMethod(1);
	m_Computation.SetAlpha((float)g_Alpha->value());
	m_Computation.SetBeta((float)g_Beta->value());
	m_Computation.SetGamma((float)g_Gamma->value());
	m_Computation.SetMaxPerturbation((float)g_MaxPerturbation->value());
	m_Computation.SetScale4NbIterations((int)g_Scale4NbIterations->value());
	m_Computation.SetScale2NbIterations((int)g_Scale2NbIterations->value());
	m_Computation.SetScale1NbIterations((int)g_Scale1NbIterations->value());
	m_Computation.SetNumBasis((float)g_NumBasis->value());
	m_Computation.SetRegistrationFilterType((const char *)g_RegistrationFilterType->value());
	m_Computation.SetDeformationFieldSmoothingSigma((float)g_DeformationFieldSmoothingSigma->value());
	m_Computation.SetPyramidLevels((int)g_PyramidLevels->value());
	m_Computation.SetMovingShrinkFactors("16,16,16");
	m_Computation.SetFixedShrinkFactors("16,16,16");
	m_Computation.SetIterationCountPyramidLevels("300,50,30,20,15");

  // Skull Stripping parameters
	g_DeleteVesselsButton->clear();
	m_Computation.SetDeleteVessels(0);
  
  // Intensity Rescaling parameters
	g_HistogramQuantileButton->set();
	g_TissueMeanMatchButton->clear();
	m_Computation.SetIntensityRescalingMethod(1);
	
  // Regional histogram	
	g_QuantilesDisp->value("1,5,33,50,66,95,99");
	m_Computation.SetQuantiles("1,5,33,50,66,95,99");
}

void AutoSegGUIControls::UpdateParameters()
{
	SetSubcorticalStructures();
	SetGenericROIMaps();
	SetParcellationMap();
	SetGridTemplateParameters();
	SetIntensityRescalingMethod();
	m_Computation.SetDataAutoSegDirectory(g_DataAutoSegDirectoryDisp->value());
	
	m_Computation.SetQuantiles(g_QuantilesDisp->value());
	m_Computation.SetPointSpacing(g_PointSpacingDisp->value());

	m_Computation.SetN4ITKBiasFieldCorrection(g_N4ITKBiasFieldCorrectionButton->value());
	m_Computation.SetNbOfIterations (g_NbOfIterations->value());
	m_Computation.SetBSplineGridResolutions (g_BSplineGridResolutions->value());
	m_Computation.SetConvergenceThreshold((float)g_ConvergenceThreshold->value());
	m_Computation.SetSplineDistance((int)g_SplineDistance->value());
	m_Computation.SetShrinkFactor((int)g_ShrinkFactor->value());
	m_Computation.SetBSplineOrder((int)g_BSplineOrder->value());
	m_Computation.SetBSplineAlpha((int)g_BSplineAlpha->value());
	m_Computation.SetBSplineBeta((float)g_BSplineBeta->value());
	m_Computation.SetHistogramSharpening(g_HistogramSharpening->value());
}

void AutoSegGUIControls::CheckDirectoryName(char *_Directory)
{
	int Length;

	Length = std::strlen(_Directory);
	if (_Directory[Length-1] != '/')
	{
		_Directory[Length] = '/';
		_Directory[Length+1] = '\0';
	}
}

void AutoSegGUIControls::RightJustifyData(const char *_Input, char *_Output)
{
	int Char1 = 0;
	int Char2 = 0;  

	if ( (!g_T2Button->value()) && (!g_PDButton->value()) )
		std::strcpy(_Output, _Input);  
	else if ( (g_T2Button->value()) && (g_PDButton->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;  
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;  

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, _Input+Char1+Char2+1); 
	}
	else 
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;    

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);      
		std::strcat(_Output,_Input+Char1);
	}
}

void AutoSegGUIControls::RightJustifyAuxData(const char *_Input, char *_Output)
{
	int Char1 = 0;
	int Char2 = 0;
	int Char3 = 0;
	int Char4 = 0;
	int Char5 = 0;
	int Char6 = 0;
	int Char7 = 0;
	int Char8 = 0;

	if ( (!g_Aux1Button->value()) && (!g_Aux2Button->value()) && (!g_Aux3Button->value()) && (!g_Aux4Button->value()) && (!g_Aux5Button->value()) && (!g_Aux6Button->value()) && (!g_Aux7Button->value()) && (!g_Aux8Button->value()) )
		std::strcpy(_Output, _Input);
	
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) && (g_Aux8Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4, 1) != 0)
			Char4++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5, 1) != 0)
			Char5++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6, 1) != 0)
			Char6++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1+Char7, 1) != 0)
			Char7++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1+Char7+1+Char8, 1) != 0)
			Char8++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1, Char4);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1, Char5);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1, Char6);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1, Char7);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1+Char7+1, Char8);
		std::strcat(_Output, _Input+Char1+Char2+Char3+Char4+Char5+Char6+Char7+Char8+7);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) && (g_Aux7Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4, 1) != 0)
			Char4++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5, 1) != 0)
			Char5++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6, 1) != 0)
			Char6++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1+Char7, 1) != 0)
			Char7++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1, Char4);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1, Char5);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1, Char6);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6+1, Char7);
		std::strcat(_Output, _Input+Char1+Char2+Char3+Char4+Char5+Char6+Char7+6);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()) && (g_Aux6Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4, 1) != 0)
			Char4++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5, 1) != 0)
			Char5++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1+Char6, 1) != 0)
			Char6++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1, Char4);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1, Char5);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5+1, Char6);
		std::strcat(_Output, _Input+Char1+Char2+Char3+Char4+Char5+Char6+5);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) && (g_Aux5Button->value()))
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4, 1) != 0)
			Char4++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4+1+Char5, 1) != 0)
			Char5++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1, Char4);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1+Char4+1, Char5);
		std::strcat(_Output, _Input+Char1+Char2+Char3+Char4+Char5+4);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) && (g_Aux4Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3+1+Char4, 1) != 0)
			Char4++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1+Char3+1, Char4);
		std::strcat(_Output, _Input+Char1+Char2+Char3+Char4+3);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) && (g_Aux3Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;
		while (std::strncmp(" ", _Input+Char1+1+Char2+1+Char3, 1) != 0)
			Char3++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1+Char2+1, Char3);
		std::strcat(_Output, _Input+Char1+Char2+Char3+2);
	}
	else if ( (g_Aux1Button->value()) && (g_Aux2Button->value()) )
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;
		while (std::strncmp(" ", _Input+Char1+1+Char2, 1) != 0)
			Char2++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output, " @r");
		std::strncat(_Output,_Input+Char1+1, Char2);
		std::strcat(_Output, _Input+Char1+Char2+1);
	}
	else
	{
		while (std::strncmp(" ", _Input+Char1, 1) != 0)
			Char1++;

		std::strcpy(_Output,"@r");
		std::strncat(_Output, _Input, Char1);
		std::strcat(_Output,_Input+Char1);
	}
}