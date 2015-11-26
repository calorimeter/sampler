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
// $Id: B4cDetectorConstruction.cc 87359 2014-12-01 16:04:27Z gcosmo $
// 
/// \file B4cDetectorConstruction.cc
/// \brief Implementation of the B4cDetectorConstruction class

#include "B4cDetectorConstruction.hh"
#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4cDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::B4cDetectorConstruction(G4UserLimits* limits, G4double absoThickness_, G4double gapThickness_, G4int noLayers, G4double hadLayerThickness_, G4int feLayers_, G4int wLayers_)
 : G4VUserDetectorConstruction(),
   fLimits(limits),
   fCheckOverlaps(true),
   fNofLayers(noLayers),
   calorSizeXY(10*cm),
   absoThickness(absoThickness_),
   gapThickness(gapThickness_),
   hadLayerThickness(hadLayerThickness_),
   fFeLayers(feLayers_),
   fWLayers(wLayers_)

{
    layerThickness = absoThickness + gapThickness;

    if(fFeLayers <= 0 && fWLayers <= 0){
    	hadLayerThickness = 0;
    }

    if(hadLayerThickness <= 0){
    	fFeLayers = 0;
    	fWLayers = 0;
    }

    if(fNofLayers <= 0){
    	absoThickness = 0;
    	gapThickness = 0;
    }
    if(absoThickness <= 0 && gapThickness <= 0){
    	fNofLayers = 0;
    }

    if(fNofLayers == 0 && fFeLayers == 0 && fWLayers == 0){
    	G4ExceptionDescription msg;
        msg << "Calorimeter does not seem to have any layers";
        G4Exception("B4DetectorConstruction::B4cDetectorConstruction()",
          "MyCode0001", FatalException, msg);
    }

    if(fFeLayers > 0 && fWLayers > 0){
    	G4ExceptionDescription msg;
        msg << "You have two different types of hadronic calorimeters!";
        G4Exception("B4DetectorConstruction::B4cDetectorConstruction()",
          "MyCode0001", FatalException, msg);
    }


    calorThickness = (fNofLayers * layerThickness) + ((fFeLayers + fWLayers) * hadLayerThickness);
    worldSizeXY = 1.2 * calorSizeXY;
    worldSizeZ  = 1.2 * calorThickness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{ 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  G4NistManager* nistManager = G4NistManager::Instance();
  //nistManager->FindOrBuildMaterial("G4_Cu");
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_W");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  //fNofLayers = 10;

  //Moved this to header file for retrieval later
  /*
  G4double absoThickness = 10.*mm;
  G4double gapThickness =  5.*mm;
  G4double calorSizeXY  = 10.*cm;

  G4double layerThickness = absoThickness + gapThickness;
  G4double calorThickness = fNofLayers * layerThickness;
  G4double worldSizeXY = 1.2 * calorSizeXY;
  G4double worldSizeZ  = 1.2 * calorThickness; */
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  G4Material* absorberMaterial = G4Material::GetMaterial("G4_Pb"); //Lead or Copper?
  G4Material* gapMaterial = G4Material::GetMaterial("liquidArgon");
  G4Material* ironMaterial = G4Material::GetMaterial("G4_Fe");
  G4Material* tungstenMaterial = G4Material::GetMaterial("G4_W");
  
  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial || ! ironMaterial || ! tungstenMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  

  //     
  // World
  //

  G4double emThickness = fNofLayers * layerThickness;
  G4double feThickness = fFeLayers * hadLayerThickness;
  G4double wThickness = fWLayers * hadLayerThickness;


  G4VSolid* worldS 
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/*/2*/); // its size
                         
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 


  G4LogicalVolume* calorLV = 0;
  G4LogicalVolume* calorFeLV = 0;
  G4LogicalVolume* calorWLV = 0;



  //
  // EM Calorimeter
  //
  if(fNofLayers > 0){
	  //
	  // Calorimeter
	  //
	  G4VSolid* calorimeterS
	    = new G4Box("Calorimeter",     // its name
	                 calorSizeXY/2, calorSizeXY/2, emThickness/2); // its size

	 calorLV = new G4LogicalVolume(
	                 calorimeterS,     // its solid
	                 defaultMaterial,  // its material
	                 "Calorimeter");   // its name

	  new G4PVPlacement(
	                 0,                // no rotation
	                 G4ThreeVector(),  // at (0,0,0)
	                 calorLV,          // its logical volume
	                 "Calorimeter",    // its name
	                 worldLV,          // its mother  volume
	                 false,            // no boolean operation
	                 0,                // copy number
	                 fCheckOverlaps);  // checking overlaps

	  //
	  // Layer
	  //
	  G4VSolid* layerS
		= new G4Box("Layer",           // its name
					 calorSizeXY/2, calorSizeXY/2, layerThickness/2); //its size

	  G4LogicalVolume* layerLV
		= new G4LogicalVolume(
					 layerS,           // its solid
					 defaultMaterial,  // its material
					 "Layer");         // its name

	  new G4PVReplica(
					 "Layer",          // its name
					 layerLV,          // its logical volume
					 calorLV,          // its mother
					 kZAxis,           // axis of replication
					 fNofLayers,        // number of replica
					 layerThickness);  // width of replica

	  //
	  // Absorber
	  //
	  if(absoThickness > 0){
	  G4VSolid* absorberS
		= new G4Box("Abso",            // its name
					 calorSizeXY/2, calorSizeXY/2, absoThickness/2); // its size

	  G4LogicalVolume* absorberLV
		= new G4LogicalVolume(
					 absorberS,        // its solid
					 absorberMaterial, // its material
					 "AbsoLV");        // its name
	  absorberLV->SetUserLimits(fLimits);

	   new G4PVPlacement(
					 0,                // no rotation
					 G4ThreeVector(0., 0., -gapThickness/2), // its position
					 absorberLV,       // its logical volume
					 "Abso",           // its name
					 layerLV,          // its mother  volume
					 false,            // no boolean operation
					 0,                // copy number
					 fCheckOverlaps);  // checking overlaps
	  }

	   //
	   // Gap
	   //
	   if(gapThickness > 0){
	  G4VSolid* gapS
		= new G4Box("Gap",             // its name
					 calorSizeXY/2, calorSizeXY/2, gapThickness/2); // its size

	  G4LogicalVolume* gapLV
		= new G4LogicalVolume(
					 gapS,             // its solid
					 gapMaterial,      // its material
					 "GapLV");         // its name
	  gapLV->SetUserLimits(fLimits);

	  new G4PVPlacement(
					 0,                // no rotation
					 G4ThreeVector(0., 0., absoThickness/2), // its position
					 gapLV,            // its logical volume
					 "Gap",            // its name
					 layerLV,          // its mother  volume
					 false,            // no boolean operation
					 0,                // copy number
					 fCheckOverlaps);  // checking overlaps
	   }
  }



   //
   // Iron Hadron Calorimeter
   //
   if(fFeLayers > 0){

		  //
		  // Calorimeter
		  //
		  G4VSolid* calorimeterFe
		    = new G4Box("CalorimeterFe",     // its name
		                 calorSizeXY/2, calorSizeXY/2, feThickness/2); // its size

		  calorFeLV = new G4LogicalVolume(
		                 calorimeterFe,     // its solid
		                 defaultMaterial,  // its material
		                 "CalorimeterFe");   // its name

		  new G4PVPlacement(
		                 0,               // no rotation
		                 G4ThreeVector(0,0,emThickness/2 + feThickness/2), // behind the em Calorimeter
		                 calorFeLV,          // its logical volume
		                 "CalorimeterFe",    // its name
		                 worldLV,          // its mother  volume
		                 false,            // no boolean operation
		                 0,                // copy number
		                 fCheckOverlaps);  // checking overlaps

		  //
		  // Layer
		  //
	   G4VSolid* layerFe
		 = new G4Box("FeLayer",           // its name
					  calorSizeXY/2, calorSizeXY/2, hadLayerThickness/2); //its size

	   G4LogicalVolume* layerFeLV
		 = new G4LogicalVolume(
					  layerFe,           // its solid
					  defaultMaterial,  // its material
					  "FeLayer");         // its name

	   new G4PVReplica(
					  "FeLayer",          // its name
					  layerFeLV,          // its logical volume
					  calorFeLV,          // its mother
					  kZAxis,           // axis of replication
					  fFeLayers,        // number of replica
					  hadLayerThickness);  // width of replica

	   G4VSolid* ironS
	     = new G4Box("iron",            // its name
	                  calorSizeXY/2, calorSizeXY/2, hadLayerThickness/2); // its size

	   G4LogicalVolume* ironLV
	     = new G4LogicalVolume(
	                  ironS,        // its solid
	                  ironMaterial, // its material
	                  "ironLV");        // its name
	   ironLV->SetUserLimits(fLimits);

	    new G4PVPlacement(
	                  0,                // no rotation
	                  G4ThreeVector(0., 0., 0), // its position
	                  ironLV,       // its logical volume
	                  "iron",           // its name
	                  layerFeLV,          // its mother  volume
	                  false,            // no boolean operation
	                  0,                // copy number
	                  fCheckOverlaps);  // checking overlaps
   }

   //
   // Tungsten Hadron Calorimeter
   //
   if(fWLayers > 0){

		  //
		  // Calorimeter
		  //
		  G4VSolid* calorimeterW
		    = new G4Box("CalorimeterW",     // its name
		                 calorSizeXY/2, calorSizeXY/2, wThickness/2); // its size

		  calorWLV = new G4LogicalVolume(
		                 calorimeterW,     // its solid
		                 defaultMaterial,  // its material
		                 "CalorimeterW");   // its name

		  new G4PVPlacement(
		                 0,               // no rotation
		                 G4ThreeVector(0,0,(emThickness/2) + (feThickness) + (wThickness/2)), // behind other calos
		                 calorWLV,          // its logical volume
		                 "CalorimeterW",    // its name
		                 worldLV,          // its mother  volume
		                 false,            // no boolean operation
		                 0,                // copy number
		                 fCheckOverlaps);  // checking overlaps

		  //
		  // Layer
		  //
	   G4VSolid* layerW
		 = new G4Box("WLayer",           // its name
					  calorSizeXY/2, calorSizeXY/2, hadLayerThickness/2); //its size

	   G4LogicalVolume* layerWLV
		 = new G4LogicalVolume(
					  layerW,           // its solid
					  defaultMaterial,  // its material
					  "WLayer");         // its name
	   layerWLV->SetUserLimits(fLimits);

	   new G4PVReplica(
					  "WLayer",          // its name
					  layerWLV,          // its logical volume
					  calorWLV,          // its mother
					  kZAxis,           // axis of replication
					  fWLayers,        // number of replica
					  hadLayerThickness);  // width of replica

	   G4VSolid* tungstenS
	     = new G4Box("tungsten",            // its name
	                  calorSizeXY/2, calorSizeXY/2, hadLayerThickness/2); // its size

	   G4LogicalVolume* tungstenLV
	     = new G4LogicalVolume(
	                  tungstenS,        // its solid
	                  tungstenMaterial, // its material
	                  "tungstenLV");        // its name

	    new G4PVPlacement(
	                  0,                // no rotation
	                  G4ThreeVector(0., 0., 0), // its position
	                  tungstenLV,       // its logical volume
	                  "tungsten",           // its name
	                  layerWLV,          // its mother  volume
	                  false,            // no boolean operation
	                  0,                // copy number
	                  fCheckOverlaps);  // checking overlaps
   }

  //
  // print parameters
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << fNofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName() 
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "as well as " << fFeLayers << " layers of: "
    << hadLayerThickness/mm << "mm of " << ironMaterial->GetName()
    << " + "
    << fWLayers << " layers of: " << hadLayerThickness/mm << "mm of "
    << tungstenMaterial->GetName() << G4endl
    << "------------------------------------------------------------" << G4endl;
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);

  if(calorLV){
	  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(0,0,1.0));
  	  simpleBoxVisAtt->SetVisibility(true);
  	  calorLV->SetVisAttributes(simpleBoxVisAtt);
  }

  if(calorFeLV){
	  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,0,0,1.0));
  	  simpleBoxVisAtt->SetVisibility(true);
  	  calorFeLV->SetVisAttributes(simpleBoxVisAtt);
  }

  if(calorWLV){
	  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(0,1.0,0,1.0));
  	  simpleBoxVisAtt->SetVisibility(true);
  	  calorWLV->SetVisAttributes(simpleBoxVisAtt);
  }
  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // 
  // Sensitive detectors
  //

  //if(absoThickness > 0){
  B4cCalorimeterSD* absoSD 
    = new B4cCalorimeterSD("AbsorberSD", "AbsorberHitsCollection", fNofLayers);
  SetSensitiveDetector("AbsoLV",absoSD);
  //}

  //if(gapThickness > 0){
  B4cCalorimeterSD* gapSD 
    = new B4cCalorimeterSD("GapSD", "GapHitsCollection", fNofLayers);
  SetSensitiveDetector("GapLV",gapSD);
  //}


  if(fFeLayers > 0){
	  B4cCalorimeterSD* ironSD
	    = new B4cCalorimeterSD("HadronicSD", "HadronicHitsCollection", fFeLayers);
	  SetSensitiveDetector("ironLV",ironSD);
  }

  if(fWLayers > 0){
	  B4cCalorimeterSD* tungstenSD
	    = new B4cCalorimeterSD("HadronicSD", "HadronicHitsCollection", fWLayers);
	  SetSensitiveDetector("tungstenLV",tungstenSD);
  }


  // 
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
