#ifndef CLUSTERING_HH
#define CLUSTERING_HH

// ROOT includes
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include <iostream>
#include <vector>
#include <algorithm>

#include "InputManager.h"
#include "Helper.h"


#include "OpticalCluster.hh"
#include "WireCluster.hh"
#include "ClusterSelector.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"


class Clustering {
public:

  Clustering():
    fInputFileName ("/dune/app/users/plasorak/workspace/SNAna.root"),
    fInputTreeName (""),
    fOutputFileName(""),    
    f_Output       (NULL),
    fTrigger       (NULL),
    fClustSelec    (NULL),
    fClustEng      (NULL),
    fPrintLevel    (-1),
    t_Output_ClusteredWireHit   (NULL),
    t_Output_ClusteredOpticalHit(NULL),
    t_Output_TrueInfo           (NULL),
    h_ENu_MC     (NULL),
    h_MarlTime_MC(NULL),
    h_TimeElapsed(NULL),
    im()
    {
      for(size_t i = 0; i < fvec_g_config.size(); i++){
        if(fvec_g_config[i]) delete fvec_g_config[i];
        fvec_g_config[i] = NULL;
      }

      SetupConfigurations_AdjChanTolerance();
      SetupConfigurations_MinHitADC       ();
      SetupConfigurations_HitsInWindow    ();
      SetupConfigurations_MinChannels     ();
      SetupConfigurations_MinChanWidth    ();
      SetupConfigurations_TimeWindowSize  ();
      SetupConfigurations_TotalADC        ();


      std::vector<size_t> sizes ={fvec_cut_HitsInWindow  .size(),
                                  fvec_cut_MinHitADC     .size(),
                                  fvec_cut_MinChannels   .size(),
                                  fvec_cut_MinChanWidth  .size(),
                                  fvec_cut_TimeWindowSize.size(),
                                  fvec_cut_TotalADC      .size()};
    
      fNConfig = (int)(*std::min_element(sizes.begin(), sizes.end()));
      std::cout << "There are " << fNConfig << " configs"<< std::endl;
      fNCuts = 6;

      for(int i = 0; i < fNConfig; i++)
      {
        std::cout << "---- Config " << i << std::endl;
        std::cout << "AdjChanTolerance[" << i << "] "
                  << fvec_cut_AdjChanTolerance[i] << std::endl;//
        std::cout << "HitsInWindow[" << i << "] "
                  << fvec_cut_HitsInWindow[i] << std::endl;//
        std::cout << "MinChannels[" << i << "] "
                  << fvec_cut_MinChannels[i] << std::endl;//
        std::cout << "MinChanWidth[" << i << "] "
                  << fvec_cut_MinChanWidth[i] << std::endl;//
        std::cout << "TimeWindowSize[" << i << "] "
                  << fvec_cut_TimeWindowSize[i] << std::endl;
        std::cout << "TotalADC[" << i << "] "
                  << fvec_cut_TotalADC[i] << std::endl;

        fvec_ClusterCount   .push_back(0);
        fvec_OptClusterCount.push_back(0);

        fvec_g_config.push_back(new TGraph(fNCuts));
        fvec_g_config.back()->SetName(Form("g_ConfigDefinitions%i",i));
        fvec_g_config.back()->SetPoint(0, 1, fvec_cut_AdjChanTolerance[i]);
        fvec_g_config.back()->SetPoint(1, 2, fvec_cut_HitsInWindow    [i]);
        fvec_g_config.back()->SetPoint(2, 3, fvec_cut_MinChannels     [i]);
        fvec_g_config.back()->SetPoint(3, 4, fvec_cut_MinChanWidth    [i]);
        fvec_g_config.back()->SetPoint(4, 5, fvec_cut_TimeWindowSize  [i]);
        fvec_g_config.back()->SetPoint(5, 6, fvec_cut_TotalADC        [i]);
      }
    };

  int         GetPrintLevel() const { return fPrintLevel;     };
  std::string GetInputFile () const { return fInputFileName;  };
  std::string GetInputTree () const { return fInputTreeName;  };
  std::string GetOutputFile() const { return fOutputFileName; };
  void        SetPrintLevel(const int p=-1)         { fPrintLevel     = p; };
  void        SetInputFile (const std::string s="") { fInputFileName  = s; };
  void        SetInputTree (const std::string s="") { fInputTreeName  = s; };
  void        SetOutputFile(const std::string s="") { fOutputFileName = s; };
    
  void SetupConfigurations_AdjChanTolerance(const std::vector<float> vec_cut_AdjChanTolerance = {1,2,2,2,2,2}          )
    { fvec_cut_AdjChanTolerance = vec_cut_AdjChanTolerance; };
  void SetupConfigurations_MinHitADC       (const std::vector<float> vec_cut_MinHitADC        = {0,0,0,0,0,0}          )
    { fvec_cut_MinHitADC = vec_cut_MinHitADC; };
  void SetupConfigurations_HitsInWindow    (const std::vector<float> vec_cut_HitsInWindow     = {2,3,3,4,5,6}          )
    { fvec_cut_HitsInWindow  = vec_cut_HitsInWindow; };
  void SetupConfigurations_MinChannels     (const std::vector<float> vec_cut_MinChannels      = {2,2,2,2,2,2}          )
    { fvec_cut_MinChannels = vec_cut_MinChannels; };
  void SetupConfigurations_MinChanWidth    (const std::vector<float> vec_cut_MinChanWidth     = {0,0,0,0,0,0}          )
    { fvec_cut_MinChanWidth = vec_cut_MinChanWidth; };
  void SetupConfigurations_TimeWindowSize  (const std::vector<float> vec_cut_TimeWindowSize   = {20,20,20,20,20,20}    )
    { fvec_cut_TimeWindowSize = vec_cut_TimeWindowSize; };
  void SetupConfigurations_TotalADC        (const std::vector<float> vec_cut_TotalADC         = {350,400,450,400,400,0})
    { fvec_cut_TotalADC =vec_cut_TotalADC; };
 
  int GetNConfig() const { return fNConfig; };
  std::vector<float> GetCutAdjChanTolerance() const { return fvec_cut_AdjChanTolerance; };
  std::vector<float> GetCutHitsInWindow    () const { return fvec_cut_HitsInWindow    ; };
  std::vector<float> GetCutMinChannels     () const { return fvec_cut_MinChannels     ; };
  std::vector<float> GetCutMinChanWidth    () const { return fvec_cut_MinChanWidth    ; };
  std::vector<float> GetCutTimeWindowSize  () const { return fvec_cut_TimeWindowSize  ; };
  std::vector<float> GetCutTotalADC        () const { return fvec_cut_TotalADC        ; };

  // The main guts...
  int ClusterAll(int);
  void FillClusterInfo(WireCluster* clust);
  void FillClusterInfo(OpticalCluster* clust);
  void FillUnusedHitInfo(WireHit* hit){ ResetFillVariable(); };
  // void DisplayAndSaveHits(const std::vector<WireHit*>&  cHitVector,
  //                         unsigned int index);
  void ResetFillVariable();

  ~Clustering()
    {
      if(t_Output_ClusteredWireHit   ) delete t_Output_ClusteredWireHit   ;
      if(t_Output_ClusteredOpticalHit) delete t_Output_ClusteredOpticalHit;
      if(t_Output_TrueInfo           ) delete t_Output_TrueInfo           ;
      
      for(size_t i = 0; i < fvec_g_config.size(); i++){
        if(fvec_g_config[i]) delete fvec_g_config[i];
        fvec_g_config[i] = NULL;
      }
      fvec_g_config.clear();
      if(fTrigger)    delete fTrigger;
      if(fClustSelec) delete fClustSelec;
      if(fClustEng)   delete fClustEng;  
      fTrigger = NULL;
      fClustSelec = NULL;
      fClustEng = NULL;

      fvec_ClusterCount.clear();
      fvec_OptClusterCount.clear();
      
      fvec_cut_MinHitADC.clear();
      fvec_cut_AdjChanTolerance.clear();
      fvec_cut_HitsInWindow.clear();
      fvec_cut_MinChannels.clear();
      fvec_cut_MinChanWidth.clear();
      fvec_cut_TimeWindowSize.clear();
      fvec_cut_TotalADC.clear();
      fvec_OptClusterCount.clear();

      t_Output_ClusteredWireHit    = NULL;
      t_Output_ClusteredOpticalHit = NULL;
      t_Output_TrueInfo            = NULL;

      if(h_ENu_MC)      delete h_ENu_MC;
      if(h_MarlTime_MC) delete h_MarlTime_MC;
      if(h_TimeElapsed) delete h_TimeElapsed;
      h_ENu_MC = NULL;
      h_MarlTime_MC = NULL;
      h_TimeElapsed = NULL;

      if(f_Output) f_Output->Close();
      f_Output = NULL;

    };
  
private:
  std::string fInputFileName ;
  std::string fInputTreeName ;
  std::string fOutputFileName;
  TFile *f_Output;
  Trigger* fTrigger;
  ClusterSelector* fClustSelec;
  ClusterEngine* fClustEng;
  std::vector<float> fvec_ClusterCount   ;
  std::vector<float> fvec_OptClusterCount;

  std::vector<float> fvec_cut_AdjChanTolerance;
  std::vector<float> fvec_cut_HitsInWindow    ;
  std::vector<float> fvec_cut_MinChannels     ;
  std::vector<float> fvec_cut_MinChanWidth    ;
  std::vector<float> fvec_cut_TimeWindowSize  ;
  std::vector<float> fvec_cut_TotalADC        ;
  std::vector<float> fvec_cut_MinHitADC       ;

  std::vector<TGraph*> fvec_g_config;
  int fNConfig;
  int fCurrentConfig;
  int fPrintLevel;
  unsigned int fNCuts;
  unsigned int fNEvent;
   
  TTree* t_Output_ClusteredWireHit;
  TTree* t_Output_ClusteredOpticalHit;
  TTree* t_Output_TrueInfo;

  TH1D* h_ENu_MC     ;
  TH1D* h_MarlTime_MC;
  TH1D* h_TimeElapsed;
  
//OUTPUT VARIABLES
  int    out_Event         ;
  int    out_MarleyIndex   ;
  int    out_Cluster       ;
  int    out_Config        ;
  int    out_StartChan     ;
  int    out_EndChan       ;
  int    out_ChanWidth     ;
  int    out_NChan         ;
  int    out_Type          ;
  int    out_NHits         ;
  float  out_SumADC        ;
  float  out_FirstTimeHit  ;
  float  out_LastTimeHit   ;
  float  out_TimeWidth     ;
  double out_E_deposited   ;
  double out_MC_UnderlyingE;
  double out_TrClusterPosX ;
  double out_TrClusterPosY ;
  double out_TrClusterPosZ ;
  double out_RecClusterPosX;
  double out_RecClusterPosY;
  double out_RecClusterPosZ;
  double out_pur_Other     ;
  double out_pur_SNnu      ;
  double out_pur_APA       ;
  double out_pur_CPA       ;
  double out_pur_Ar39      ;
  double out_pur_Neutron   ;
  double out_pur_Krypton   ;
  double out_pur_Polonium  ;
  double out_pur_Radon     ;
  double out_pur_Ar42      ;
  double out_YWidth        ;
  double out_ZWidth        ;
  double out_SumPE         ;
  
  std::vector<int>    out_HitView;
  std::vector<int>    out_GenType;
  std::vector<int>    out_HitChan;
  std::vector<double> out_HitTime;
  std::vector<double> out_HitSADC;
  std::vector<double> out_HitSPE;
  std::vector<double> out_HitRMS;
  std::vector<double> out_HitWidth;
  std::vector<double> out_TrPosX; 
  std::vector<double> out_TrPosY; 
  std::vector<double> out_TrPosZ; 

  std::vector<double> out_MarlTime;
  std::vector<double> out_ENu;
  std::vector<double> out_ENu_Lep;
  std::vector<double> out_PosX;
  std::vector<double> out_PosY;
  std::vector<double> out_PosZ;
  std::vector<double> out_PosT;
  std::vector<double> out_DirX;
  std::vector<double> out_DirY;
  std::vector<double> out_DirZ;

  InputManager im;

};
#endif

 
