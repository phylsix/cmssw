#include "DQMOffline/Trigger/plugins/DiDispStaMuonMonitor.h"

// -----------------------------
//  constructors and destructor
// -----------------------------

DiDispStaMuonMonitor::DiDispStaMuonMonitor( const edm::ParameterSet& iConfig ) :
  folderName_             ( iConfig.getParameter<std::string>("FolderName") )
  , muonToken_             ( consumes<reco::TrackCollection>       (iConfig.getParameter<edm::InputTag>("muons")     ) )
  , muonPt_variable_binning_ ( iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<std::vector<double> >("muonPtBinning") )
  , muonPt_binning_          ( getHistoPSet   (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("muonPtPSet")    ) )
  , muonEta_binning_          ( getHistoPSet   (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("muonEtaPSet")    ) )
  , muonPhi_binning_          ( getHistoPSet   (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("muonPhiPSet")    ) )
  , ls_binning_           ( getHistoPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("lsPSet")     ) )
  , num_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("numGenericTriggerEventPSet"),consumesCollector(), *this))
  , den_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("denGenericTriggerEventPSet"),consumesCollector(), *this))
  , muonSelection_ ( iConfig.getParameter<std::string>("muonSelection") )
  , nmuons_     ( iConfig.getParameter<unsigned int>("nmuons" )     )
{

  muonPtME_.numerator   = nullptr;
  muonPtME_.denominator = nullptr;
  muonPtME_variableBinning_.numerator   = nullptr;
  muonPtME_variableBinning_.denominator = nullptr;
  muonPtVsLS_.numerator   = nullptr;
  muonPtVsLS_.denominator   = nullptr;
  muonEtaME_.numerator   = nullptr;
  muonEtaME_.denominator = nullptr;
  muonPhiME_.numerator   = nullptr;
  muonPhiME_.denominator = nullptr;

  subMuonPtME_.numerator   = nullptr;
  subMuonPtME_.denominator = nullptr;
  subMuonPtME_variableBinning_.numerator   = nullptr;
  subMuonPtME_variableBinning_.denominator = nullptr;
  subMuonEtaME_.numerator   = nullptr;
  subMuonEtaME_.denominator = nullptr;
  subMuonPhiME_.numerator   = nullptr;
  subMuonPhiME_.denominator = nullptr;


}

DiDispStaMuonMonitor::~DiDispStaMuonMonitor() = default;

DiDispStaMuonMonitor::MEbinning DiDispStaMuonMonitor::getHistoPSet(const edm::ParameterSet & pset)
{
  return MEbinning{
    pset.getParameter<unsigned int>("nbins"),
      pset.getParameter<double>("xmin"),
      pset.getParameter<double>("xmax"),
      };
}

DiDispStaMuonMonitor::MEbinning DiDispStaMuonMonitor::getHistoLSPSet(const edm::ParameterSet & pset)
{
  return MEbinning{
    pset.getParameter<unsigned int>("nbins"),
      0.,
      double(pset.getParameter<unsigned int>("nbins"))
      };
}

void DiDispStaMuonMonitor::setTitle(DiDispStaMuonME& me, const std::string& titleX, const std::string& titleY, bool bookDen)
{
  me.numerator->setAxisTitle(titleX,1);
  me.numerator->setAxisTitle(titleY,2);
  if(bookDen) {
    me.denominator->setAxisTitle(titleX,1);
    me.denominator->setAxisTitle(titleY,2);
  }

}

void DiDispStaMuonMonitor::bookME(DQMStore::IBooker &ibooker, DiDispStaMuonME& me, const std::string& histname, const std::string& histtitle, int nbins, double min, double max)
{
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, min, max);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, min, max);
}
void DiDispStaMuonMonitor::bookME(DQMStore::IBooker &ibooker, DiDispStaMuonME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binning)
{
  int nbins = binning.size()-1;
  std::vector<float> fbinning(binning.begin(),binning.end());
  float* arr = &fbinning[0];
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, arr);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, arr);
}
void DiDispStaMuonMonitor::bookME(DQMStore::IBooker &ibooker, DiDispStaMuonME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, double ymin, double ymax, bool bookDen)
{
  me.numerator   = ibooker.bookProfile(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, ymin, ymax);
  if(bookDen) me.denominator = ibooker.bookProfile(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, ymin, ymax);
}
void DiDispStaMuonMonitor::bookME(DQMStore::IBooker &ibooker, DiDispStaMuonME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, int nbinsY, double ymin, double ymax)
{
  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, nbinsY, ymin, ymax);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, nbinsY, ymin, ymax);
}
void DiDispStaMuonMonitor::bookME(DQMStore::IBooker &ibooker, DiDispStaMuonME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX, const std::vector<double>& binningY)
{
  int nbinsX = binningX.size()-1;
  std::vector<float> fbinningX(binningX.begin(),binningX.end());
  float* arrX = &fbinningX[0];
  int nbinsY = binningY.size()-1;
  std::vector<float> fbinningY(binningY.begin(),binningY.end());
  float* arrY = &fbinningY[0];

  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, arrX, nbinsY, arrY);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, arrX, nbinsY, arrY);
}

void DiDispStaMuonMonitor::bookHistograms(DQMStore::IBooker     & ibooker,
				 edm::Run const        & iRun,
				 edm::EventSetup const & iSetup)
{

  std::string histname, histtitle;
  bool bookDen;

  std::string currentFolder = folderName_ ;
  ibooker.setCurrentFolder(currentFolder);

  histname = "muonPt"; histtitle = "muonPt";
  bookDen = true;
  bookME(ibooker,muonPtME_,histname,histtitle,muonPt_binning_.nbins,muonPt_binning_.xmin, muonPt_binning_.xmax);
  setTitle(muonPtME_,"DisplacedStandAlone Muon p_{T} [GeV]","Events / [GeV]", bookDen);

  histname = "muonPt_variable"; histtitle = "muonPt";
  bookDen = true;
  bookME(ibooker,muonPtME_variableBinning_,histname,histtitle,muonPt_variable_binning_);
  setTitle(muonPtME_variableBinning_,"DisplacedStandAlone Muon p_{T} [GeV]","Events / [GeV]", bookDen);

  histname = "muonPtVsLS"; histtitle = "muonPt vs LS";
  bookDen = true;
  bookME(ibooker,muonPtVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,muonPt_binning_.xmin, muonPt_binning_.xmax, bookDen);
  setTitle(muonPtVsLS_,"LS","DisplacedStandAlone Muon p_{T} [GeV]", bookDen);

  histname = "muonEta"; histtitle = "muonEta";
  bookDen = true;
  bookME(ibooker,muonEtaME_,histname,histtitle,muonEta_binning_.nbins,muonEta_binning_.xmin, muonEta_binning_.xmax);
  setTitle(muonEtaME_,"DisplacedStandAlone Muon #eta","Events", bookDen);

  histname = "muonPhi"; histtitle = "muonPhi";
  bookDen = true;
  bookME(ibooker,muonPhiME_,histname,histtitle,muonPhi_binning_.nbins,muonPhi_binning_.xmin, muonPhi_binning_.xmax);
  setTitle(muonPhiME_,"DisplacedStandAlone Muon #phi","Events", bookDen);


  //-----------------------
  //                      |
  //-----------------------

  if (nmuons_>1) {
    histname = "subMuonPt"; histtitle = "subMuonPt";
    bookDen = true;
    bookME(ibooker,subMuonPtME_,histname,histtitle,muonPt_binning_.nbins,muonPt_binning_.xmin, muonPt_binning_.xmax);
    setTitle(subMuonPtME_,"Subleading DisplacedStandAlone Muon p_{T} [GeV]","Events / [GeV]", bookDen);

    histname = "subMuonPt_variable"; histtitle = "subMuonPt";
    bookDen = true;
    bookME(ibooker,subMuonPtME_variableBinning_,histname,histtitle,muonPt_variable_binning_);
    setTitle(subMuonPtME_variableBinning_,"Subleading DisplacedStandAlone Muon p_{T} [GeV]","Events / [GeV]", bookDen);

    histname = "subMuonEta"; histtitle = "subMuonEta";
    bookDen = true;
    bookME(ibooker,subMuonEtaME_,histname,histtitle,muonEta_binning_.nbins,muonEta_binning_.xmin, muonEta_binning_.xmax);
    setTitle(subMuonEtaME_,"Subleading DisplacedStandAlone Muon #eta","Events", bookDen);

    histname = "subMuonPhi"; histtitle = "subMuonPhi";
    bookDen = true;
    bookME(ibooker,subMuonPhiME_,histname,histtitle,muonPhi_binning_.nbins,muonPhi_binning_.xmin, muonPhi_binning_.xmax);
    setTitle(subMuonPhiME_,"Subleading DisplacedStandAlone Muon #phi","Events", bookDen);
  }


  // Initialize the GenericTriggerEventFlag
  if ( num_genTriggerEventFlag_ && num_genTriggerEventFlag_->on() ) num_genTriggerEventFlag_->initRun( iRun, iSetup );
  if ( den_genTriggerEventFlag_ && den_genTriggerEventFlag_->on() ) den_genTriggerEventFlag_->initRun( iRun, iSetup );

}

void DiDispStaMuonMonitor::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup)  {

  // Filter out events if Trigger Filtering is requested
  if (den_genTriggerEventFlag_->on() && ! den_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;

  int ls = iEvent.id().luminosityBlock();


  edm::Handle<reco::TrackCollection> DSAHandle;
  iEvent.getByToken( muonToken_, DSAHandle );
  if ((unsigned int)(DSAHandle->size()) < nmuons_ ) return;
  std::vector<reco::Track> muons;
  for ( auto const & m : *DSAHandle ) {
    if ( muonSelection_( m ) ) muons.push_back(m);
  }
  if ((unsigned int)(muons.size()) < nmuons_ ) return;
  double muonPt = -999;
  double muonEta = -999;
  double muonPhi = -999;
  if(!muons.empty()){
    muonPt = muons[0].pt();
    muonEta = muons[0].eta();
    muonPhi = muons[0].phi();
  }

  // filling histograms (denominator)
  muonPtME_.denominator -> Fill(muonPt);
  muonPtME_variableBinning_.denominator -> Fill(muonPt);
  muonEtaME_.denominator -> Fill(muonEta);
  muonPhiME_.denominator -> Fill(muonPhi);
  muonPtVsLS_.denominator-> Fill(ls, muonPt);

  if (nmuons_>1) {
    subMuonPtME_.denominator  -> Fill(muons[1].pt());
    subMuonPtME_variableBinning_.denominator -> Fill(muons[1].pt());
    subMuonEtaME_.denominator -> Fill(muons[1].eta());
    subMuonPhiME_.denominator -> Fill(muons[1].phi()); 
  }

  // filling histograms (numerator)
  if (num_genTriggerEventFlag_->on() && ! num_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;
  muonPtME_.numerator -> Fill(muonPt);
  muonPtME_variableBinning_.numerator -> Fill(muonPt);
  muonPtVsLS_.numerator -> Fill(ls, muonPt);
  muonEtaME_.numerator -> Fill(muonEta);
  muonPhiME_.numerator -> Fill(muonPhi);

  if (nmuons_>1) {
    subMuonPtME_.denominator  -> Fill(muons[1].pt());
    subMuonPtME_variableBinning_.denominator -> Fill(muons[1].pt());
    subMuonEtaME_.denominator -> Fill(muons[1].eta());
    subMuonPhiME_.denominator -> Fill(muons[1].phi()); 
  }

}

void DiDispStaMuonMonitor::fillHistoPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<unsigned int>   ( "nbins", 200);
  pset.add<double>( "xmin", -0.5 );
  pset.add<double>( "xmax", 19999.5 );
}

void DiDispStaMuonMonitor::fillHistoLSPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<unsigned int>   ( "nbins", 2000);
}

void DiDispStaMuonMonitor::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<std::string>  ( "FolderName", "HLT/DiDispStaMuon" );

  desc.add<edm::InputTag>( "muons",    edm::InputTag("displacedStandAloneMuons") );
  desc.add<std::string>("muonSelection", "pt > 0");
  desc.add<unsigned int>("nmuons",     2);

  edm::ParameterSetDescription genericTriggerEventPSet;
  genericTriggerEventPSet.add<bool>("andOr");
  genericTriggerEventPSet.add<edm::InputTag>("dcsInputTag", edm::InputTag("scalersRawToDigi") );
  genericTriggerEventPSet.add<std::vector<int> >("dcsPartitions",{});
  genericTriggerEventPSet.add<bool>("andOrDcs", false);
  genericTriggerEventPSet.add<bool>("errorReplyDcs", true);
  genericTriggerEventPSet.add<std::string>("dbLabel","");
  genericTriggerEventPSet.add<bool>("andOrHlt", true);
  genericTriggerEventPSet.add<edm::InputTag>("hltInputTag", edm::InputTag("TriggerResults::HLT") );
  genericTriggerEventPSet.add<std::vector<std::string> >("hltPaths",{});
  genericTriggerEventPSet.add<std::string>("hltDBKey","");
  genericTriggerEventPSet.add<bool>("errorReplyHlt",false);
  genericTriggerEventPSet.add<unsigned int>("verbosityLevel",1);

  desc.add<edm::ParameterSetDescription>("numGenericTriggerEventPSet", genericTriggerEventPSet);
  desc.add<edm::ParameterSetDescription>("denGenericTriggerEventPSet", genericTriggerEventPSet);

  edm::ParameterSetDescription histoPSet;
  edm::ParameterSetDescription muonPtPSet;
  edm::ParameterSetDescription muonEtaPSet;
  edm::ParameterSetDescription muonPhiPSet;
  edm::ParameterSetDescription lsPSet;
  fillHistoPSetDescription(muonPtPSet);
  fillHistoPSetDescription(muonEtaPSet);
  fillHistoPSetDescription(muonPhiPSet);
  fillHistoPSetDescription(lsPSet);
  histoPSet.add<edm::ParameterSetDescription>("muonPtPSet", muonPtPSet);
  histoPSet.add<edm::ParameterSetDescription>("muonEtaPSet", muonEtaPSet);
  histoPSet.add<edm::ParameterSetDescription>("muonPhiPSet", muonPhiPSet);
  histoPSet.add<edm::ParameterSetDescription>("lsPSet", lsPSet);
  std::vector<double> bins = {0.,20.,40.,60.,80.,90.,100.,110.,120.,130.,140.,150.,160.,170.,180.,190.,200.,220.,240.,260.,280.,300.,350.,400.,450.,1000.};
  histoPSet.add<std::vector<double> >("muonPtBinning", bins);

  desc.add<edm::ParameterSetDescription>("histoPSet",histoPSet);

  descriptions.add("DiDispStaMuonMonitoring", desc);
}

// Define this as a plug-in
DEFINE_FWK_MODULE(DiDispStaMuonMonitor);
