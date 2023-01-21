#pragma once

//#indef  PROFILMAP
//#define PROFILMAP

#include <stdio.h>  
#include <string>
#include <vector>
#include <map>
#include "EnOceanRawValue.h"

//Profile descriptor from eep.xml

#if 0
typedef struct _T_DATAFIELD {
	int Offset;
	int Size;
	double  RangeMin;
	double  RangeMax;
	double  ScaleMin;
	double  ScaleMax;

	std::string  ShortCut;
	std::string  description;
	std::string  enumerate       ;


  std::string toString() 
  {
    char buf[1024] ;
    _snprintf ( buf,sizeof(buf),"%2d;%2d;%5.2f;%5.2f;%5.2f;%5.2f;%-10s;%s;%s", 
	 Offset,
	 Size,
	 RangeMin,
	 RangeMax,
	 ScaleMin,
	 ScaleMax,
	 ShortCut.c_str(),
	 description.c_str(),
     enumerate.c_str()
        );
   return buf;

  }
}T_DATAFIELD;
typedef struct _T_PROFIL_LIST {
  int Profil ; 
  int Rorg;
  int Func;
  int type;
  T_DATAFIELD* *cases  ;
  std::string FuncTitle ;
  std::string TypeTitle ;



  std::string toString() 
  {
    char buf[1024] ;
    _snprintf ( buf,sizeof(buf),"%06x;%2x;%2x;%2x;%-10s;%s", 
   Profil , 
   Rorg,
   Func,
   type,
	 FuncTitle.c_str(),
	 TypeTitle.c_str() );
   return buf;

  }
}T_PROFIL_LIST  ;

#endif

//descriptor for a eep case
//a profil a as several case 
// a case as several datafield
typedef std::vector<T_DATAFIELD> T_DATAFIELDS;

class T_EEP_CASE 
{
public:
    T_DATAFIELDS dataFileds;
	std::string Title;
	std::string Desc ;
    unsigned int size() { return dataFileds.size(); }

  void print() 
  {
     printf ("Case :  %s \n", getName());
    for (unsigned int i=0;i<dataFileds.size();i++)
      printf("%s\n",this->dataFileds.at(i).toString().c_str() );
  }
  T_DATAFIELD * FindShortCut(const char * shorcut )
  {
    for (unsigned int i=0;i<dataFileds.size();i++)
      if (  this->dataFileds.at(i).ShortCut ==  shorcut )
        return &this->dataFileds.at(i);
    return 0;
  }
  void setName(std::string& pname) { Title = pname; }
  char* getName() { return (char*)Title.c_str(); }

  void AddDataField(T_DATAFIELD& dataf)
  {
   dataFileds.push_back(dataf);
   }

  T_DATAFIELD* at(uint32_t ind)
  {
      return &dataFileds.at(ind);
  }

};

//list of cases for a profil
typedef std::vector<T_EEP_CASE> T_EEP_CASES;


//Profile descriptor from eep.xml
class T_PROFIL_EEP 
{
public:

  int Profil ; 
  int Rorg;
  int Func;
  int type;
  std::string FuncTitle ;
  std::string TypeTitle ;

  T_EEP_CASES cases ;

  void AddDataField (  unsigned int CaseNb  , T_DATAFIELD & dataf  )
  {
    if ( !(CaseNb<cases.size()) )
    {
      T_EEP_CASE newCase ;
      cases.push_back(newCase);
    }
    //add data field
    cases[CaseNb].AddDataField(dataf);
  };

  void AddProfil (int pProfil ,   std::string pFuncTitle ,  std::string pTypeTitle  )
  {
    Profil = pProfil;
    FuncTitle = pFuncTitle ;
    TypeTitle = pTypeTitle ;
  };
  void printCase( int CaseNb )
  {
    printf ("Case : %d : %s \n",CaseNb, cases[CaseNb].getName());
    for (unsigned int i=0;i<cases[CaseNb].size();i++ )
      printf ("Case:%d = %s\n",CaseNb,cases[CaseNb].dataFileds[i].toString().c_str() );
  }
  void printCases()
  {
    printf("Profil:%06X %s %s \n",Profil,FuncTitle.c_str(),TypeTitle.c_str() );
    for (unsigned int i=0;i<cases.size();i++ )
       printCase(i);
  }

};


typedef std::map<unsigned int, T_PROFIL_EEP > T_PROFIL_EEP_MAP;

class T_PROFIL_MAP 
{
	  T_PROFIL_EEP_MAP lProfils  ;
    T_EEP_CASE emptyCase ;

public:

  void AddProfil (int pProfil ,   std::string pFuncTitle ,  std::string pTypeTitle  )
  {
    lProfils[pProfil].AddProfil( pProfil ,   pFuncTitle ,  pTypeTitle);
  };

   void AddDataField ( int pProfil , int CaseNb  , T_DATAFIELD & dataf  )
   {
      lProfils[pProfil].AddDataField ( CaseNb ,  dataf  ) ;
   }
   void print()
   {
    for (T_PROFIL_EEP_MAP::iterator itt = lProfils.begin(); itt != lProfils.end(); itt++)
		{
      itt->second.printCases();
		}

   }

   T_EEP_CASE * getCase ( int pProfil , unsigned int CaseNb )
   {
     if ( CaseNb < lProfils[pProfil].cases.size() )
     {
       return &lProfils[pProfil].cases[CaseNb] ;
     }
     else
       return &emptyCase;

   }

   T_PROFIL_EEP * getProfil (int profil)
   {
     return &lProfils[profil] ;
   }

   void parseEEP_xml(const char* eepXmlFineName, const char* prorg, const char* pfctnum, FILE* out);

   void LoadXml()
   {
//    FILE*  stdnull = fopen("eep.txt", "w");
    FILE*  stdnull = fopen("nul.txt", "w");
//    stdnull = stdout;
    if(lProfils.size() == 0 ) 
        parseEEP_xml( "eep.xml", "0x","0x", stdnull);
   }
};

extern     T_PROFIL_MAP Profils;

//#endif
