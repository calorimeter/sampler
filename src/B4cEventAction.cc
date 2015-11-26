//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4cEventAction.cc 89037 2015-03-18 09:25:01Z gcosmo $
// 
/// \file B4cEventAction.cc
/// \brief Implementation of the B4cEventAction class

#include "B4cEventAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"
#include "B4cDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction()
 : G4UserEventAction(),
   fAbsHCID(-1),
   fGapHCID(-1),
   fHcalHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection* 
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  B4cCalorHitsCollection* hitsCollection 
    = static_cast<B4cCalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
                              G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength,
                              G4double hcalEdep, G4double hcalTrackLength) const
{
  // print event statistics
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl
     << "        Gap: total energy: " 
     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
     << "        HCAL: total energy: "
     << std::setw(7) << G4BestUnit(hcalEdep, "Energy")
     << "       total track length: "
     << std::setw(7) << G4BestUnit(hcalTrackLength, "Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  
  // Get hits collections IDs (only once)
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
    fGapHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("GapHitsCollection");
    fHcalHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("HadronicHitsCollection");
  }

  B4cDetectorConstruction* construct = (B4cDetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();

  //Possibility of element given as 0 for homogeneous detector
  G4bool hasAbso = construct->GetAbsorberThickness() > 0;
  G4bool hasGap = construct->GetGapThickness() > 0;
  G4bool hasHCAL = construct->GetNumberOfHadronicLayers() > 0;

  G4double absoEdep = 0; G4double absoTrackLength = 0;
  G4double  gapEdep = 0; G4double  gapTrackLength = 0;
  G4double hcalEdep = 0; G4double hcalTrackLength = 0;

  B4cCalorHitsCollection* absoHC = 0;
  B4cCalorHitsCollection* gapHC = 0;
  B4cCalorHitsCollection* hcalHC = 0;

  if(hasAbso){ //Set proper values for absorber hits
	  absoHC = GetHitsCollection(fAbsHCID, event);
	  B4cCalorHit* absoHit = (*absoHC)[absoHC->entries()-1];
	  absoEdep = absoHit->GetEdep();
	  absoTrackLength = absoHit->GetTrackLength();
  }

  if(hasGap){ //Set proper values for gap hits
	  gapHC = GetHitsCollection(fGapHCID, event);
	  B4cCalorHit* gapHit = (*gapHC)[gapHC->entries()-1];
	  gapEdep = gapHit->GetEdep();
	  gapTrackLength = gapHit->GetTrackLength();
  }

  if(hasHCAL){ //Set proper values for HCAL
	  hcalHC = GetHitsCollection(fHcalHCID, event);
	  B4cCalorHit* hcalHit = (*hcalHC)[hcalHC->entries()-1];
	  hcalEdep = hcalHit->GetEdep();
	  hcalTrackLength = hcalHit->GetTrackLength();
  }


  // Print per event (modulo n)
  //
  G4int eventID = event->GetEventID();
  G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     

    PrintEventStatistics(
      absoEdep, absoTrackLength,
      gapEdep, gapTrackLength,
      hcalEdep, hcalTrackLength);
  }  
  
  // Fill histograms, ntuple
  //

  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  

  for(int i = 0; i < construct->GetNumberOfLayers(); i++){
	  G4double x,y,r;
	  G4double eDep = 0;

	  //Passive material would not be known in real-world applications
	  if(hasAbso){
		  eDep += (*absoHC)[i]->GetEdep();
	  }

	  if(hasGap){
		  G4ThreeVector gPos = (*gapHC)[i]->GetPosition();
		  x = gPos.getX(); y = gPos.getY(); 
		  r = sqrt((x*x) + (y*y));
		  analysisManager->FillH1(1,r,(*gapHC)[i]->GetEdep());
		  eDep += (*absoHC)[i]->GetEdep();
	  }

	  G4int lGap =  i + 1;
	  if(hasGap) analysisManager->FillH1(2, lGap, (*gapHC)[i]->GetEdep());
  }


  // fill ntuple

  analysisManager->FillNtupleDColumn(0, absoEdep + gapEdep);
  analysisManager->AddNtupleRow();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
