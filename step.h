/*
* This file is part of OpenBrew.  It was modified to work with myBrew.
*
* OpenBrew is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OpenBrew is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OpenBrew. If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (c) 2012 James McLean <james.mclean@gmail.com>
*
* myBrew Copyright (c) 2015 Bill Klinefelter <bill.klinefelter@gmail.com>
*
*/

#ifndef STEP_H
#define STEP_H

#include <QString>
#include "defines.h"

class Step
{
protected:
   signed int stepVessel;
   signed int stepNumber;
   signed int stepLength;
   signed int stepType;
   signed int stepRampTime;
   signed int infusionAmount; // water to add to the mashtun.
   signed int stepActive;
   float stepTemperature;
   float stepEndTemperature;
   bool promptForGrain;
   QString stepName;
public:
    Step();

    signed int getStepLength() const {
        return stepLength;
    }
    void setStepLength(signed int stepLength) {
        this->stepLength = stepLength;
    }
    signed int getStepNumber() const {
        return stepNumber;
    }
    void setStepNumber(signed int stepNumber) {
        this->stepNumber = stepNumber;
    }
    signed int getStepType() const {
        return stepType;
    }
    void setStepType(signed int stepType) {
        this->stepType = stepType;
    }
    signed int getVessel() const {
        return stepVessel;
    }
    void setVessel(signed int vessel) {
        this->stepVessel = vessel;
    }
    QString getStepName() const {
        return stepName;
    }
    float getStepTemperature() const {
        return stepTemperature;
    }
    void setStepTemperature(float stepTemperature) {
        this->stepTemperature = stepTemperature;
    }
    bool getPromptForGrain() const {
        return promptForGrain;
    }
    void setPromptForGrain(bool promptForGrain) {
        this->promptForGrain = promptForGrain;
    }
    void setStepName(const QString& stepName) {
        this->stepName = stepName;
    }
};

#endif // STEP_H
