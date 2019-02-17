#include "beerxmlrecipe.h"

BeerXMLRecipe::BeerXMLRecipe(const char *recipePath)
{
    // set the recipe path
   setRecipeFullPath(recipePath);
   qDebug() << "Parsing recipe: " << getRecipeFullPath() << "\n";
   // Parse the recipe and generate vessels and steps for each vessel
   parseRecipe();
   // Setup the objects for each vessel.
   Hlt hlt;
   Mlt mlt;
   Kettle kettle;
}

BeerXMLRecipe::~BeerXMLRecipe()
{

}

/* parseRecipe() code borrowed and modified from
* This file is part of OpenBrew.
* OpenBrew is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* Copyright (c) 2012 James McLean <james.mclean@gmail.com>
*/

int BeerXMLRecipe::parseRecipe() {
    int i;
    QString recipeCorrect;
    QString yesLower;
    QString yesUpper;
    QString stepName;
    double boilLength;
    double stepTemp;
    double stepLength;
    double stepNumber;
    Step current_mash_step;
    Hop current_hop;
    Fermentable current_fermentable;
    //yesLower = 'y';
    //yesUpper = 'Y';
    try {
        pugi::xml_document doc;
        if (!doc.load_file(getRecipeFullPath())) {
            return -1;
        }
        pugi::xml_node recipe_name = doc.child("RECIPES").child("RECIPE").child("NAME");
        qDebug() << "Parsing recipe: " << recipe_name.child_value() << "\n"; // this returns the recipe name correctly.
        this->recipe_name = recipe_name.child_value();
        pugi::xml_node recipe_version = doc.child("RECIPES").child("RECIPE").child("VERSION");
        this->version = recipe_version.child_value();
        pugi::xml_node recipe_type = doc.child("RECIPES").child("RECIPE").child("TYPE");
        this->recipe_type = recipe_type.child_value();
        pugi::xml_node recipe_brewer = doc.child("RECIPES").child("RECIPE").child("BREWER");
        this->brewer_name = recipe_brewer.child_value();
        pugi::xml_node recipe_batchsize = doc.child("RECIPES").child("RECIPE").child("BATCH_SIZE");
        this->batch_size = atof(recipe_batchsize.child_value());
        pugi::xml_node recipe_boilsize = doc.child("RECIPES").child("RECIPE").child("BOIL_SIZE");
        this->boil_size = atof(recipe_boilsize.child_value());
        pugi::xml_node boil_time = doc.child("RECIPES").child("RECIPE").child("BOIL_TIME");
        this->boil_time = atof(boil_time.child_value());
        pugi::xml_node boil_efficiency = doc.child("RECIPES").child("RECIPE").child("EFFICIENCY");
        this->efficiency = atof(boil_efficiency.child_value());

        // get the hop additions from the recipe
        pugi::xml_node recipe_hop_additions = doc.child("RECIPES").child("RECIPE").child("HOPS");
        i=0;
        for (pugi::xml_node hop = recipe_hop_additions.child("HOP"); hop; hop = hop.next_sibling("HOP"))
        {
            // Get some details of this step from the recipe.
            current_hop.name = hop.child("NAME").child_value();
            current_hop.version = hop.child("VERSION").child_value();
            current_hop.origin = hop.child("ORIGIN").child_value();
            current_hop.alpha = atof(hop.child("ALPHA").child_value());
            current_hop.amount = atof(hop.child("AMOUNT").child_value());
            current_hop.use = hop.child("USE").child_value();
            current_hop.time = atof(hop.child("TIME").child_value());
            current_hop.type = hop.child("TYPE").child_value();
            current_hop.display_amount = hop.child("DISPLAY_AMOUNT").child_value();
            this->hop_additions.push_back(current_hop);
            i++;
        }
        // get the mash steps from the recipe
        pugi::xml_node mash_steps = doc.child("RECIPES").child("RECIPE").child("MASH").child("MASH_STEPS");
        // add the mash steps to the batch
        i = 0;
        for (pugi::xml_node step = mash_steps.child("MASH_STEP"); step; step = step.next_sibling("MASH_STEP"))
        {
            // Get some details of this step from the recipe.
            if (i==0)
            {
                // first mash step means capture strike temp and amount
                QString strike_temp_text = QString::fromStdString(step.child("INFUSE_TEMP").child_value());
                strike_temp_text.chop(2);
                this->strike_temp = atof(strike_temp_text.toLocal8Bit());
                this->strike_amount = atof(step.child("INFUSE_AMOUNT").child_value());
                this->strike_description = step.child("DESCRIPTION").child_value();
                QString mash_thickness_text = QString::fromStdString(step.child("WATER_GRAIN_RATIO").child_value());
                mash_thickness_text.chop(6);
                this->mash_thickness = atof(mash_thickness_text.toLocal8Bit());
            }
            current_mash_step.setStepName(step.child("NAME").child_value());
            current_mash_step.setStepTemperature(atof(step.child("STEP_TEMP").child_value()));
            current_mash_step.setStepLength(atof(step.child("STEP_TIME").child_value()));
            current_mash_step.setStepNumber(i);
            this->mash_steps.push_back(current_mash_step);
            i++;
        }

        // get pitch temp for primary fermentation
        pugi::xml_node recipe_pitch_temp = doc.child("RECIPES").child("RECIPE").child("PRIMARY_TEMP");
        this->pitch_temp = atof(recipe_pitch_temp.child_value());

        // get beer style
        pugi::xml_node recipe_style = doc.child("RECIPES").child("RECIPE").child("STYLE");
        this->style_name = recipe_style.child("NAME").child_value();

        // get list of fermentables
        pugi::xml_node recipe_fermentables = doc.child("RECIPES").child("RECIPE").child("FERMENTABLES");
        i=0;
        for (pugi::xml_node fermentable = recipe_fermentables.child("FERMENTABLE"); fermentable; fermentable = fermentable.next_sibling("FERMENTABLE"))
        {
            // Get some details of this step from the recipe.
            current_fermentable.name = fermentable.child("NAME").child_value();
            current_fermentable.version = fermentable.child("VERSION").child_value();
            current_fermentable.amount = atof(fermentable.child("AMOUNT").child_value());
            current_fermentable.display_color = fermentable.child("DISPLAY_COLOR").child_value();
            this->fermentables.push_back(current_fermentable);
            this->grain_weight+= current_fermentable.amount;
            i++;
        }
        // convert grain weight from kg to lb
        this->grain_weight *= 2.20462;

        // get yeast info
        pugi::xml_node recipe_yeast = doc.child("RECIPES").child("RECIPE").child("YEASTS").child("YEAST");
        this->yeast_name = recipe_yeast.child("NAME").child_value();


    } catch (exception& e) {
        // handle the exceptions generated.
        qDebug() << "Something went wrong loading the XML file" << "\n";
    }
    qDebug() << "Recipe parsing completed. " << "\n";
    qDebug() << "insert recipe here according to the steps in each vessel " << "\n";
    //qDebug() << "Does this recipe look correct? (Y/n):" << "\n";
    //cin >> recipeCorrect;
    //if(recipeCorrect.compare(yesLower) != 0 || recipeCorrect.compare(yesUpper) != 0) {
    //    return 1;
    //}
    qDebug() << "\n";
    qDebug() << "=================================" << "\n";
    qDebug() << "\n";
    return 0;
}

void BeerXMLRecipe::addStep(Step step) {
    qDebug() << "Adding Step '" << step.getStepName() << "' to vessel " << step.getVessel() << "\n";
    if(step.getVessel() == VESSEL_HLT) {
        hlt.addStep(step);
    }
    if(step.getVessel() == VESSEL_MLT) {
        mlt.addStep(step);
    }
    if(step.getVessel() == VESSEL_KETTLE) {
        kettle.addStep(step);
    }
}
Vessel BeerXMLRecipe::getVessel(int vessel) {
    switch(vessel) {
    case VESSEL_HLT:
        hlt.setVesselType(vessel);
        return hlt;
        break;
    case VESSEL_MLT:
        mlt.setVesselType(vessel);
        return mlt;
        break;
    case VESSEL_KETTLE:
        kettle.setVesselType(vessel);
        return kettle;
        break;
    }
    // Fall back and return the HLT.
    return hlt;
}
