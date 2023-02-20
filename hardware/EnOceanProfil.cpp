#include "stdafx.h"

//Profile descriptor from eep.xml
#include <stdarg.h>  
#include "../tinyxpath/tinyxml.h"
#include "../tinyxpath/xpath_static.h"
#include "../main/Helper.h"
#include "EnOceanProfil.h"
unsigned long StrToULong(std::string value)
{
	try
	{
		std::size_t p = value.find("x");
		if (p!=std::string::npos)
		{
			value[p] = '0';
			return std::stoul(value, 0, 16);
		}
		p = value.find("b");
		if (p!=std::string::npos)
		{
			value[p] = '0';
			return std::stoul(value, 0, 2);
		}
		return std::stoul(value, 0, 10);
	}
	catch(...)
	{
		return 0 ;
	}
}
std::string  getText(TiXmlElement *l_item , const char * name , const char * defaultText = "" )
{
  TiXmlElement *l_child = l_item->FirstChildElement(name )  ;
  if (l_child) {
    const char * text =  l_child->GetText();
    if (text != 0 )
      return text ;
    else
      return defaultText ;
  }
  else
    return defaultText ;
}
TiXmlElement*  xmlFindNode (TiXmlElement* l_item, const char * pXmlPath )
{
	std::vector<std::string> splitresults;
	StringSplit(pXmlPath, "/", splitresults);
    int len = strlen(pXmlPath);
    bool attributePresent = (pXmlPath[len - 1] != '/');
    int NbNodes = splitresults.size();
    if (attributePresent)
        NbNodes--;
	
    for (int i=0;i<NbNodes;i++) 
	{
        const char* name = splitresults[i].c_str();
        if (name[0] )
        {
		    l_item =  l_item->FirstChildElement(name )  ;
            if (l_item == 0)
                break;
        }
	}
    return l_item;
}
std::string  xmlGetNodeText (TiXmlElement* l_item, const char * pXmlPath , const char * defaultText = "")
{
    const char* text = "";
    TiXmlElement* l_node = xmlFindNode ( l_item, pXmlPath );
    if (l_item != 0)
    {
        text =  l_item->GetText();
        if (text == 0 )
          text = defaultText ;
    }
    return text;
}
std::string  xmlGetNodeAttribute (TiXmlElement* l_item, const char * pXmlPath , const char * AttributeName , const char * defaultText = "")
{
    const char* text = "";
    TiXmlElement* l_node = xmlFindNode ( l_item, pXmlPath );
    if (l_item == 0)
    {
        text = l_item->Attribute( AttributeName ) ;
        if (text != 0 )
          text = defaultText ;
    }
    return text;
}
void replaceChar (std::string & title , char car , char repCar )
{
    for (uint32_t i=0;i<title.size();i++ ){
    if (title[i]==car) 
      title[i]=repCar;
    }
}
std::string  getEnum(TiXmlElement *l_p , int nbc, std::string & enumerates)
{
  std::string enumerate ;
char tab[256];
memset(tab,' ',sizeof(tab));
if (nbc>=sizeof(tab) )
  nbc=sizeof(tab)-1;
tab[nbc]=0;

//tab[0]=0;
enumerates = "";
enumerate  = ",{";

TiXmlElement *l_enum  = l_p->FirstChildElement( "enum" );
if (l_enum==0) 
{
  return "" ;
}

TiXmlElement *l_item  = l_enum->FirstChildElement( "item" );

while(l_item)
{
  TiXmlElement *lv =  l_item->FirstChildElement( "value" );

  if (lv==0)
  {
    enumerate+="}";
    return enumerate ;
  }

  if ( l_item->FirstChildElement( "description" ) )
  {
      std::string svalue       = getText( l_item, "value" ) ;
      std::string Description = getText( l_item, "description" ) ;
      replaceChar(Description,'"',' ');
      enumerates += svalue + ":" + Description + ";";

//    enumerate += std_format ( "%s//Value: %s = %s \n",tab, lv->GetText()    , l_item->FirstChildElement( "description" )->GetText()   );
      unsigned long  value =  StrToULong( svalue);
     enumerate += std_format ( "{ %lu , \"%s\" },",value    , Description.c_str()   );
  tab[0]=' ';
  }
  l_item =  l_item->NextSiblingElement("item");
}
    enumerate+="}";

return enumerate ;
}
std::string getTab(uint32_t tabLen ,  std::string & word)
{
  std::string tab;
  if (tabLen >  word.size() ) 
    tab.insert (0, tabLen - word.size()  ,' ' );
  return tab ;

}

class Element  
{
  std::string field ;
  TiXmlElement* lpElement ;
  Element(const char * pfield)
  {
    field = pfield;
  }
  void First()
  {
  }

};
/*
              <datafield>
                <data>Supply voltage</data>
                <shortcut>SVC</shortcut>
                <description>Supply voltage (linear)</description>
                <info/>
                <bitoffs>0</bitoffs>
                <bitsize>8</bitsize>
                <range>
                  <min>0</min>
                  <max>255</max>
                </range>
                <scale>
                  <min>0</min>
                  <max>5.1</max>
                </scale>
              < unit>V</unit>
              </datafield>
*/

int axtoi( const char * svalue  )
{
  int value ;
  sscanf( svalue ,"%x",&value ) ;
  return value;

}
int axtoi( std::string  & svalue  )
{
  return axtoi(svalue.c_str());
}
std::string getDataFieldName (int Profil, int caseNb  )
{
//                    if ( ( Profil>>16 )!= 0xA5 )
                    return std_format("%06X_CMD%d",Profil,caseNb );
//                  else
//                    return std_format("%06X",Profil,caseNb );
}
std::string getCaseName (int Profil, int caseNb  )
{
//                    if ( ( Profil>>16 )!= 0xA5 )
                    return std_format("%06X_CASE%d",Profil,caseNb );
//                  else
//                    return std_format("%06X",Profil,caseNb );
}
void T_PROFIL_MAP::parseEEP_xml(const char * eepXmlFineName , const char * prorg, const char * pfctnum , FILE * out , FILE * outext ,const char* spaceName )
{
int Rorg ;
int FuncNumber ;
int TypeNumber ;
std::string rorg ;
std::string funcNumber ;
std::string typeNumber ;
std::string  functtl ;
std::string  typettl ;
TiXmlDocument doc(eepXmlFineName );
if(!doc.LoadFile()){
      printf( "erreur lors du chargement");
      printf( "error #%d %s" , doc.ErrorId() , doc.ErrorDesc() ) ;
      return ;
    }
std::vector<std::string> ProfilList  ;

TiXmlElement *l_pRootElement = doc.RootElement();
 
if( NULL != l_pRootElement )
{
    {
        TiXmlElement *l_pprofile = l_pRootElement->FirstChildElement( "profile" );
        TiXmlElement *l_rorg = l_pprofile->FirstChildElement( "rorg" );
        while( l_rorg )
        {
          rorg = getText (l_rorg,"number") ;
          Rorg = axtoi(rorg);
          if (strstr(rorg.c_str(),prorg)!=0)
//          if (strcmp(rorg,"0xD2")==0)
          {
          TiXmlElement *l_pfunc = l_rorg->FirstChildElement( "func" );
          while( l_pfunc )
          {
              funcNumber = getText (l_pfunc,"number") ;
              FuncNumber = axtoi(funcNumber);
              functtl = getText (l_pfunc,"title"); 
              if (    (strstr(pfctnum,funcNumber.c_str())==0)
                   && (pfctnum != "" )
                  )
              {
                l_pfunc = l_pfunc->NextSiblingElement( "func" );
                continue;
              }
              fprintf (out,"// function number :%s : %s \n",funcNumber.c_str(),functtl.c_str());
              if(out != outext)
              fprintf (outext,"// function number :%s : %s \n",funcNumber.c_str(),functtl.c_str());
              TiXmlElement *l_ptype = l_pfunc->FirstChildElement( "type" );
              while( l_ptype )
              {
                typeNumber    = getText (l_ptype,"number") ;
                TypeNumber = axtoi(typeNumber);
                typettl       = getText (l_ptype,"title"); 
                int Profil = Rorg *256 * 256  + FuncNumber * 256 + TypeNumber;
                AddProfil(Profil,functtl,typettl);
              fprintf (out,"// function type :%s : %s \n",typeNumber.c_str(),typettl.c_str());
              if(out != outext)
                  fprintf (outext,"// function type :%s : %s \n",typeNumber.c_str(),typettl.c_str());
              fprintf(out, "//{ %s, %s, %s, \"%-80s\" , \"%-80s\" },\n",rorg.c_str(),funcNumber.c_str(),typeNumber.c_str(),functtl.c_str(),typettl.c_str() );
                //get ref :
/*			<ref>
				<rorg>D2</rorg>
				<func>01</func>
				<type>00</type>
			</ref>
*/
                int RefProfil = 0 ;
                TiXmlElement *l_pref = l_ptype->FirstChildElement( "ref" );
                if (l_pref){
                  int rorg = axtoi ( getText (l_pref,"rorg" ).c_str() );
                  int func = axtoi ( getText (l_pref,"func" ).c_str() );
                  int type = axtoi ( getText (l_pref,"type" ).c_str() );
                  RefProfil = rorg *256 * 256  + func * 256 + type ;
                  getProfil(Profil)->cases = getProfil(RefProfil)->cases ;
                }
                TiXmlElement *l_pcase = l_ptype->FirstChildElement( "case" );
                int caseNb=0;
                while( l_pcase )
                {
                  caseNb++;
                  TiXmlElement *l_pdatafield = l_pcase->FirstChildElement( "datafield" );
                  std::string TitleCase       = getText( l_pcase, "title" ) ;
                  std::string DescriptionCase = getText( l_pcase, "description" ) ;
                  replaceChar(TitleCase,'"',' ');
                  replaceChar(DescriptionCase,'"',' ');
                  std::string DataFieldName  = getDataFieldName ( Profil,  caseNb  ) ;
                  std::vector<std::string> OffsetId ;
                  fprintf (out,"\n// TITLE:%s\n",TitleCase.c_str() );
                  if(out != outext)
                  fprintf (outext,"\n// TITLE:%s\n",TitleCase.c_str() );
                  fprintf (out,  "// DESC :%s\n",DescriptionCase.c_str() );
                  fprintf (outext,"extern %s T_DATAFIELD %s [] ;\n",spaceName,DataFieldName.c_str() );
                  fprintf (out,"%s T_DATAFIELD %s [] = {\n",spaceName,DataFieldName.c_str() );
                  int bitoffs=0;
                  int bitsize=0;
                  while( l_pdatafield )
                  {
                    T_DATAFIELD dataf ;
                    //memset(&dataf,0,sizeof(T_DATAFIELD) );

                    std::string  datas = getText(l_pdatafield,"data","");
                    if (!datas.empty()  )
                    {

                      replaceChar(datas,'"',' ');

                      std::string Bitoffs="0" ;
                      std::string Bitsize="0" ;
                      Bitoffs = l_pdatafield->FirstChildElement("bitoffs" )->GetText()  ;
                      Bitsize = l_pdatafield->FirstChildElement("bitsize" )->GetText() ;
      
                      bitoffs = atoi(  Bitoffs.c_str() );
                      bitsize = atoi(  Bitsize.c_str() );

                      std::string shortcut = getText( l_pdatafield,"shortcut" ,"")  ;
                      if (shortcut.empty() ){
                        shortcut=datas;
                      }
                      replaceChar(shortcut,'.','_');replaceChar(shortcut,' ','_');replaceChar(shortcut,'-','_');replaceChar(shortcut,'(','_');replaceChar(shortcut,')','_');replaceChar(shortcut,'/','_');

                      //test range
                      std::string RangeMin ="0";
                      std::string RangeMax ="0";

                      std::string ScaleMin ="0";
                      std::string ScaleMax ="0";


                      TiXmlElement * l_prange   = l_pdatafield->FirstChildElement( "range" );
                      if (l_prange){
                         RangeMin = getText(l_prange,"min","0") ;
                         RangeMax = getText(l_prange,"max","0") ;
                      }

                      l_prange   = l_pdatafield->FirstChildElement( "scale" );
                      if (l_prange){
                         ScaleMin = getText(l_prange,"min","0") ;
                         ScaleMax = getText(l_prange,"max","0") ;
                      }

                      dataf.Offset = bitoffs ;
                      dataf.Size   = bitsize ;
                      dataf.RangeMin = atoi(RangeMin.c_str() );
                      dataf.RangeMax = atoi(RangeMax.c_str() );
                      dataf.ScaleMin = atoi(ScaleMin.c_str() );
                      dataf.ScaleMax = atoi(ScaleMax.c_str() );
                      dataf.description = datas ;

                      //recherche si existe pour ne pas avoir de define identique 
                      T_DATAFIELD * df =  getCase (Profil, caseNb-1)->FindShortCut( shortcut.c_str() );
                      if (df!=0){
                        std::string sname ;
                        int nb= 1 ;
                        do
                        {
                          sname = std_format("%s%d",shortcut.c_str(),nb++);
                           df =  getCase (Profil, caseNb-1)->FindShortCut( sname.c_str() );
                        }
                        while ( df != 0 );
                        shortcut = sname;
                      }
                      dataf.ShortCut = shortcut.c_str()  ;

                      char*nptr;
//                      int tabn = fprintf(out, "{ %2s ,%2s , \"%s\"%s , %3s , %3s , %3s , %3s , \"%s\"}," ,Bitoffs.c_str() , Bitsize.c_str(),shortcut.c_str(),getTab(8,shortcut).c_str(), RangeMin.c_str() ,RangeMax.c_str(),ScaleMin.c_str(),ScaleMax.c_str() , datas.c_str() );
                      int tabn = fprintf(out, "{ %2s ,%2s , %5.0f , %5.0f , %5.0f , %5.0f , \"%s\"%s , \"%s\"" ,Bitoffs.c_str() , Bitsize.c_str(), strtod (RangeMin.c_str(), &nptr) ,strtod (RangeMax.c_str(), &nptr),strtod (ScaleMin.c_str(), &nptr),strtod (ScaleMax.c_str(), &nptr) ,shortcut.c_str(),getTab(8,shortcut).c_str(), datas.c_str() );
                      std::string enumerateList="";
                      std::string  enumerate =  getEnum(l_pdatafield,tabn , enumerateList) ;
                      fprintf(out, "%s" , enumerate.c_str());

                      fprintf(out, "},\n" );                      
                      //                      fprintf(out, "//enum=%s\n", enumerateList.c_str());
                      dataf.enumerate = enumerateList;
                      AddDataField (Profil, caseNb-1 ,  dataf );
                      getCase(Profil, caseNb - 1)->setName(TitleCase);

                      OffsetId.push_back(shortcut);

                    }
                    l_pdatafield = l_pdatafield->NextSiblingElement( "datafield" );
                  }
                  fprintf(out,"{  0 , 0 , 0          , 0           }\n" );
                  fprintf(out,"};\n\n");
                  //end of datafields
                  fprintf(out,"%sT_EEP_CASE_ %s ={ %s , \"%s\" , \"%s\" } ;\n"  ,spaceName, getCaseName(Profil,caseNb).c_str() ,getDataFieldName(Profil,caseNb).c_str()  ,TitleCase.c_str() ,DescriptionCase.c_str() );
                  fprintf(outext,"// Index of field\n");
                  for (uint32_t i=0;i<OffsetId.size();i++)
                    fprintf(outext,"#define %s_%-10s %d\n",DataFieldName.c_str(),OffsetId[i].c_str(),i );
                  fprintf  (outext,"#define %s_%-10s %d\n",DataFieldName.c_str(),"NB_DATA",OffsetId.size() );

                  bitoffs += bitsize;
                  bitoffs = (bitoffs+7)/8 ;
                  fprintf(outext,"#define %s_%-10s %d\n",DataFieldName.c_str(),"DATA_SIZE",bitoffs );
                  //    Profils.getProfil(Profil)->printCases();
                  l_pcase = l_pcase->NextSiblingElement( "case" );
                }
                //fin du profile : list des case du profil courant
                fprintf (outext,"\nextern %s T_EEP_CASE_* %06X_CASES [] ;\n",spaceName,Profil );
                fprintf (out,"\n%s T_EEP_CASE_* %06X_CASES [] = {\n",spaceName,Profil );
                for (int i=0;i<caseNb;i++)
                   fprintf(out,"&%s ,\n",getCaseName(Profil,i+1).c_str() );
                if ( (caseNb==0) && (RefProfil!=0))
                {
                  T_PROFIL_EEP * profil = getProfil (RefProfil);
                  caseNb = profil->cases.size();
                  for (uint32_t i=0;i<profil->cases.size() ;i++)
                     fprintf(out,"&%s ,\n",getCaseName(RefProfil,i+1).c_str() );
                }
                fprintf (out,"{0 }\n" );
                fprintf(out,"};\n\n");
                ProfilList.push_back(  std_format ( "{ 0x%06X, %s , %s, %s, %06X_CASES ,%d, \"%-80s\" , \"%-80s\" },\n",Profil, rorg.c_str(),funcNumber.c_str(),typeNumber.c_str(), Profil, caseNb,  functtl.c_str(),typettl.c_str() )   );
                l_ptype = l_ptype->NextSiblingElement( "type" );
              }
              l_pfunc = l_pfunc->NextSiblingElement( "func" );
          }
          }
          l_rorg = l_rorg->NextSiblingElement( "rorg" );
        }
        fprintf (out,"\n// Profils list \n");
        fprintf (out,"T_PROFIL_LIST Profillist [] = {\n" );
        for (uint32_t i=0;i<ProfilList.size();i++)
          fprintf(out, ProfilList[i].c_str() );
        fprintf (out,"{0,0,0,0,0,0,\"\",\"\" }\n" );
        fprintf (out,"};\n\n" );
    }
}
}
T_PROFIL_MAP Profils;
// TITLE:
T_DATAFIELD A50201 [] = {
{ 28 , 1 ,    0 ,   0 ,   0 ,   0 ,"LRNB"     ,  "LRN Bit"},//Value: 0 = Teach-in telegram 
                                                           //Value: 1 = Data telegram 
{ 16 , 8 ,  255 ,   0 , -40 ,   0 ,"TMP"      , "Temperature"},
{  0 , 0 , 0    , 0   , 0   ,   0 ,""         ,""            }
};

void testProfils()
{
    T_PROFIL_MAP Profils;
  Profils.AddProfil(0xd20101,"0xd20101","ggg" );
  Profils.print();
  Profils.AddDataField (0xd20101, 0 , A50201 [0] );
  Profils.print();
  Profils.AddDataField (0xd20101, 0 , A50201 [1] );
  Profils.print();
  Profils.AddDataField (0xd20101, 0 , A50201 [2] );
  Profils.print();
  T_EEP_CASE * Case = Profils.getCase(0xd20101,0) ;
  Case->print();
}
void testProfils2()
{
    T_PROFIL_MAP Profils;
    FILE*  stdnull = fopen("EnOceanEepProfil.cpp", "w");
    FILE*  stdext = fopen("EnOceanEepProfil.h", "w");
//    FILE*  stdnull = fopen("nul.txt", "w");
//    stdnull = stdout;
    Profils.parseEEP_xml( "eep.xml", "0xD2","0x01 0x05 ", stdnull,stdext);
//    parseEEP_xml("0xD2","0x", stdnull);
  T_EEP_CASE * Case = Profils.getCase(0xd20100,0) ;
  Case->print();
   Case = Profils.getCase(0xd20101,0) ;
  Case->print();
  fclose(stdnull);
  fclose(stdext);
}
void testXml()
{
TiXmlDocument doc( "eep.xml" );
if(!doc.LoadFile()){
      printf( "erreur lors du chargement");
      printf( "error #%d %s" , doc.ErrorId() , doc.ErrorDesc() ) ;
      return ;
    }
TiXmlElement *l_pRootElement = doc.RootElement();
const char* 
pXmlPath = "/eep/profile/rorg/number/";
std::string 
res  = xmlGetNodeText( (TiXmlElement* )&doc, pXmlPath,  "UNK");printf("%s:%s\n", pXmlPath, res.c_str());
}
