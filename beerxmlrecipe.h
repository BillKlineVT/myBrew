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

#ifndef BEERXMLRECIPE_H
#define BEERXMLRECIPE_H

#include "pugixml.hpp"
#include "step.h"
#include "vessel.h"
#include <QString>
#include <QDebug>
#include "defines.h"
#include <QMainWindow>

#include "hlt.h"
#include "mlt.h"
#include "kettle.h"
#include "hop.h"
#include "fermentable.h"

class BeerXMLRecipe
{

protected:
    Vessel hlt;
    Vessel mlt;
    Vessel kettle;
    const char *recipeFullPath;


public:
    BeerXMLRecipe(const char *recipePath);
    ~BeerXMLRecipe();
    int parseRecipe();

    Vessel getVessel(int vessel);
    void addStep(Step step);
    void run();
    void runSteps(Vessel vessel);
    const char* getRecipeFullPath() const {
        return recipeFullPath;
    }
    void setRecipeFullPath(const char* recipeFullPath) {
        this->recipeFullPath = recipeFullPath;
    }
    QString recipe_name;
    QString recipe_type;
    QString brewer_name;
    QString version;
    double batch_size;
    double boil_time;
    double boil_size;
    double efficiency;
    vector <Hop> hop_additions;
    vector <Step> mash_steps;
    vector <Fermentable> fermentables;
    double pitch_temp;
    QString style_name;
    QString yeast_name;
    double strike_temp;
    double strike_amount;
    QString strike_description;
    double mash_thickness;
    double grain_weight;
};

#endif // BEERXMLRECIPE_H
