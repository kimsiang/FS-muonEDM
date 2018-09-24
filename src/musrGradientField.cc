/***************************************************************************
 *  musrSim - the program for the simulation of (mainly) muSR instruments. *
 *          More info on http://lmu.web.psi.ch/simulation/index.html .     *
 *          musrSim is based od Geant4 (http://geant4.web.cern.ch/geant4/) *
 *                                                                         *
 *  Copyright (C) 2009 by Paul Scherrer Institut, 5232 Villigen PSI,       *
 *                                                       Switzerland       *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              *
 ***************************************************************************/

#include "globals.hh"
#include "G4GeometryManager.hh"
#include "musrGradientField.hh"

musrGradientField::musrGradientField(G4double EMF[6], G4double half_X, G4double half_Y, G4double half_Z, G4LogicalVolume* lv, G4ThreeVector c)
                : F04ElementField(c, lv)  {
  // EMF[6] ... constant vector of the field:  3 components magnetic + 3 components electric field
  // half_X, half_Y, half_Z  ... half dimenstions of the box, within which the field is defined

  for (int i = 0; i < 6; i++){
    EMfield[i] = EMF[i];
  }
  
  fieldLength = 2.*half_Z;
  fieldWidth  = 2.*half_X;
  fieldHeight = 2.*half_Y;

  G4cout << "\n-----------------------------------------------------------"
         << "\n      Uniform electromagnetic field"
         << G4endl;
   
  G4String volName = lv->GetName().substr(4);
  G4cout << "\n ---> EM field in volume " << volName << " set to:" << G4endl;
  printf   ("      B = (%0.3g, %0.3g, %0.3g) T,  E = (%0.3g, %0.3g, %0.3g) kV/mm\n",
  EMF[0]/CLHEP::tesla,         EMF[1]/CLHEP::tesla,         EMF[2]/CLHEP::tesla, 
  EMF[3]/(CLHEP::kilovolt/CLHEP::mm), EMF[4]/(CLHEP::kilovolt/CLHEP::mm), EMF[5]/(CLHEP::kilovolt/CLHEP::mm));
  G4cout << "-----------------------------------------------------------" << G4endl;
}

void musrGradientField::addFieldValue(const G4double point[4],
                                           G4double field[6]) const
{
   G4ThreeVector global(point[0],point[1],point[2]);
   G4ThreeVector local;
   
   local = global2local.TransformPoint(global);
   
   if (isOutside(local)) return;

   G4double Bx = EMfield[0];
   G4double By = EMfield[1];
   G4double Bz = EMfield[2];
   
   G4double fieldIndex = EMfield[3]*1000;

   G4double posR = std::sqrt(std::pow(point[0],2) + std::pow(point[1],2));

  if (posR>1.5e2 && posR<5.0e2){
    Bx = - EMfield[2]*fieldIndex*point[0]*point[2]*std::pow(1.5e2/posR,fieldIndex)/(posR*posR);
    By = - EMfield[2]*fieldIndex*point[1]*point[2]*std::pow(1.5e2/posR,fieldIndex)/(posR*posR);
    Bz =  EMfield[2]*std::pow(1.5e2/posR,fieldIndex);
  }
  
  else if (posR>5.0e2){
    Bx = 0*CLHEP::tesla;
    By = 0*CLHEP::tesla;
    Bz = 0*CLHEP::tesla;
  }

   G4ThreeVector B(Bx, By, Bz);
   G4ThreeVector E(0,0,0);
   
   B = global2local.Inverse().TransformAxis(B);
   E = global2local.Inverse().TransformAxis(E);

   field[0] += B[0];
   field[1] += B[1];
   field[2] += B[2];
   
   field[3] += E[0];
   field[4] += E[1];
   field[5] += E[2];
      
//  printf ("   EM field components:  B = (%0.3g, %0.3g, %0.3g) T,  E = (%0.3g, %0.3g, %0.3g) kV/mm\n",
//   field[0]/CLHEP::tesla,         field[1]/CLHEP::tesla,         field[2]/CLHEP::tesla,
//   field[3]/(CLHEP::kilovolt/CLHEP::mm), field[4]/(CLHEP::kilovolt/CLHEP::mm), field[5]/(CLHEP::kilovolt/CLHEP::mm));
}



G4double musrGradientField::GetNominalFieldValue() {
  G4double val = std::sqrt(EMfield[0]*EMfield[0]+EMfield[1]*EMfield[1]+EMfield[2]*EMfield[2]+
			   EMfield[3]*EMfield[3]+EMfield[4]*EMfield[4]+EMfield[5]*EMfield[5]);
  return val;
}

void musrGradientField::SetNominalFieldValue(G4double newFieldValue){
  EMfield[0] *= newFieldValue;
  EMfield[1] *= newFieldValue;
  EMfield[2] *= newFieldValue;
  EMfield[3] *= newFieldValue;
  EMfield[4] *= newFieldValue;
  EMfield[5] *= newFieldValue;
}

G4bool musrGradientField::isOutside(G4ThreeVector& local) const
{
  return (std::fabs(local.z()) > fieldLength/2.0 || std::fabs(local.x()) > fieldWidth/2.0 || std::fabs(local.y()) > fieldHeight/2.0);
}

G4bool musrGradientField::isWithin(G4ThreeVector& local) const
{
  return (std::fabs(local.z()) < fieldLength/2.0 && std::fabs(local.x()) < fieldWidth/2.0 && std::fabs(local.y()) < fieldHeight/2.0);
}
