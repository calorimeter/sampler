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
// $Id: exampleB4c.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleB4c.cc
/// \brief Main program of the B4c example

#include "B4cDetectorConstruction.hh"
#include "B4cActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UserSpecialCuts.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "FTFP_BERT.hh"

#include "Randomize.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleB4c [-m macro ] [-u UIsession] [-emlayers nr] "
    	<< "[-absorber <absorber thickness (mm)>] [-gap <gap thickness (mm)>] "
    	<< "[-felayers nr] [-wlayers nr] [-hadronic <layer thickness (mm)>]"
    	<< G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 15 ) {
    PrintUsage();
    return 1;
  }
  
  G4String macro;
  G4String session;

  G4int layers = 10;
  G4double absoSize = 10*mm;
  G4double gapSize = 5*mm;
  G4int feLayers = 0;
  G4int wLayers = 0;
  G4int hadSize = 20*mm;

  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if ( G4String(argv[i]) == "-emlayers" ) layers = G4UIcommand::ConvertToInt(argv[i+1]);
    else if ( G4String(argv[i]) == "-absorber" ) absoSize = G4UIcommand::ConvertToDouble(argv[i+1]);
    else if ( G4String(argv[i]) == "-gap" ) gapSize = G4UIcommand::ConvertToDouble(argv[i+1]);
    else if ( G4String(argv[i]) == "-felayers" ) feLayers = G4UIcommand::ConvertToInt(argv[i+1]);
    else if ( G4String(argv[i]) == "-wlayers" ) wLayers = G4UIcommand::ConvertToInt(argv[i+1]);
    else if ( G4String(argv[i]) == "-hadronic" ) hadSize = G4UIcommand::ConvertToDouble(argv[i+1]);
    else {
      PrintUsage();
      return 1;
    }
  }  
  
  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
  G4UserLimits* limits = new G4UserLimits(DBL_MAX, DBL_MAX, DBL_MAX, 2*MeV);
  B4cDetectorConstruction* detConstruction = new B4cDetectorConstruction(
		  limits, absoSize, gapSize, layers, hadSize, feLayers, wLayers
		  );
  runManager->SetUserInitialization(detConstruction);

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  runManager->SetUserInitialization(physicsList);
    
  B4cActionInitialization* actionInitialization
     = new B4cActionInitialization();
  runManager->SetUserInitialization(actionInitialization);
  
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

  delete limits;
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
