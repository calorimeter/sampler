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
// $Id: B4cDetectorConstruction.hh 75215 2013-10-29 16:07:06Z gcosmo $
// 
/// \file B4cDetectorConstruction.hh
/// \brief Definition of the B4cDetectorConstruction class

#ifndef B4cDetectorConstruction_h
#define B4cDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4UserLimits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In ConstructSDandField() sensitive detectors of B4cCalorimeterSD type
/// are created and associated with the Absorber and Gap volumes.
/// In addition a transverse uniform magnetic field is defined 
/// via G4GlobalMagFieldMessenger class.

class B4cDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    B4cDetectorConstruction(G4UserLimits* limits,
    		G4double absoThickness_ = 10*mm, G4double gapThickness_ = 5*mm, G4int noLayers = 10,
    		G4double hadLayerThickness_ = 20*mm, G4int feLayers_ = 0, G4int wLayers_ = 0
    		);

    virtual ~B4cDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // get methods
    G4double GetAbsorberThickness() const;
    G4double GetGapThickness() const;
    G4double GetCalorimeterSizeXY() const;
    G4int GetNumberOfLayers() const;
    G4int GetNumberOfHadronicLayers() const;
    G4double GetEMLayerThickness() const;
    G4double GetHadLayerThickness() const;
    G4double GetCalorimeterThickness() const;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
  
    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; 
                                      // magnetic field messenger

    G4UserLimits* fLimits;

    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
    G4int   fNofLayers;     // number of layers


    //Useful geometry that we might want to retrieve later

    const G4double calorSizeXY;

    G4double absoThickness; //10 mm
    G4double gapThickness;  //5 mm
    G4double layerThickness; //15 mm
    G4double hadLayerThickness; //20 mm
    G4double calorThickness;
    G4double worldSizeXY;
    G4double worldSizeZ;

    G4int   fFeLayers;      // number of layers (hadronic calorimeter, iron)
    G4int   fWLayers;       // number of layers (hadronic calorimeter, tungsten)
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline G4double B4cDetectorConstruction::GetAbsorberThickness() const {
  return absoThickness;
}

inline G4double B4cDetectorConstruction::GetGapThickness() const {
  return gapThickness;
}

inline G4double B4cDetectorConstruction::GetCalorimeterSizeXY() const {
  return calorSizeXY;
}

inline G4int B4cDetectorConstruction::GetNumberOfLayers() const {
  return fNofLayers;
}

inline G4int B4cDetectorConstruction::GetNumberOfHadronicLayers() const {
  return fFeLayers + fWLayers;
}

inline G4double B4cDetectorConstruction::GetEMLayerThickness() const {
  return layerThickness;
}

inline G4double B4cDetectorConstruction::GetHadLayerThickness() const {
  return hadLayerThickness;
}

inline G4double B4cDetectorConstruction::GetCalorimeterThickness() const {
  return calorThickness;
}


#endif

