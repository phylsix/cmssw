import FWCore.ParameterSet.Config as cms

from DQMOffline.Trigger.DiDispStaMuonMonitoring_cfi import DiDispStaMuonMonitoring

hltDiDispStaMuonMonitoring = DiDispStaMuonMonitoring.clone()
hltDiDispStaMuonMonitoring.FolderName = cms.string('HLT/EXO/DiDispStaMuon/DoubleL2Mu23NoVtx_2Cha/')
hltDiDispStaMuonMonitoring.histoPSet.lsPSet = cms.PSet(
  nbins = cms.uint32 ( 250 ),
  xmin  = cms.double(    0.),
  xmax  = cms.double( 2500.),
)
hltDiDispStaMuonMonitoring.histoPSet.muonPtPSet = cms.PSet(
    nbins = cms.uint32(50),
    xmin  = cms.double(-0.5),
    xmax  = cms.double(99.5),
    )
hltDiDispStaMuonMonitoring.histoPSet.muonEtaPSet = cms.PSet(
    nbins = cms.uint32(48),
    xmin  = cms.double(-2.4),
    xmax  = cms.double(2.4),
    )
hltDiDispStaMuonMonitoring.histoPSet.muonPhiPSet = cms.PSet(
    nbins = cms.uint32(64),
    xmin  = cms.double(-3.2),
    xmax  = cms.double(3.2),
    )
hltDiDispStaMuonMonitoring.histoPSet.muonDxyPSet = cms.PSet(
    nbins = cms.uint32(50),
    xmin  = cms.double(-120.),
    xmax  = cms.double(120.),
    )

hltDiDispStaMuonMonitoring.muons     = cms.InputTag("displacedStandAloneMuons")
hltDiDispStaMuonMonitoring.nmuons     = cms.uint32(2)

hltDiDispStaMuonMonitoring.muonSelection = cms.PSet(
    general = cms.string("hitPattern.muonStationsWithValidHits() > 1"),
    pt      = cms.string("pt > 2 "),
    dxy     = cms.string("dxy > 5"),
    )

hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.andOr         = cms.bool( False )
#hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.dbLabel       = cms.string("ExoDQMTrigger") # it does not exist yet, we should consider the possibility of using the DB, but as it is now it will need a label per path !                                                                                                           
hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.andOrHlt      = cms.bool(True)# True:=OR; False:=AND 
hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.hltInputTag   = cms.InputTag( "TriggerResults::reHLT" )
hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.hltPaths      = cms.vstring("HLT_DoubleL2Mu23NoVtx_2Cha_v*") # HLT_ZeroBias_v*
hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.errorReplyHlt = cms.bool( False )
hltDiDispStaMuonMonitoring.numGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.andOr         = cms.bool( False )
hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.dcsPartitions = cms.vint32 ( 24, 25, 26, 27, 28, 29 ) # 24-27: strip, 28-29: pixel, we should add all other detectors !
hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.andOrDcs      = cms.bool( False )
hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.errorReplyDcs = cms.bool( True )
hltDiDispStaMuonMonitoring.denGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

#hltDiDispStaMuonMonitoring.muonSelection = cms.string("hitPattern.muonStationsWithValidHits() > 1")
