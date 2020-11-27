#ifndef HELPER_CLASS_
#define HELPER_CLASS_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>
#include "BasicSc2Bot.h"
#include "BotFunctions.h"

using namespace sc2;

class Builder : public sc2::Agent{
public:
    

    void setVars(const ObservationInterface* obs, ActionInterface* act, std::vector<Point2D> curBases) {
        observation = obs;
        actions = act;
        botFunctions->setVars(obs, act);
        bases = curBases;
        
    }

    void BuildStructures() {
        
        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) >= 1) {
            for (auto base : bases) {
                const Unit *commandCenterAtLocation = botFunctions->FindNearestUnit(base, UNIT_TYPEID::TERRAN_COMMANDCENTER, Unit::Alliance::Self);
                if (!commandCenterAtLocation) {
                    continue;
                }
                else if (DistanceSquared2D(commandCenterAtLocation->pos, base) < 10){
                    //There is a command center at the start location or near it then we can build buildings around that command center.
                    //Allows us to have multiple bases with their own economies
                    buildingPoint = base;
                    TryBuildSupplyDepot();
                    TryBuildBarracks();
                    TryBuildCommandCenter();
                    TryBuildEngineeringBay();
                    TryBuildFactory();
                    TryBuildArmory();
                    TryBuildTurrets();
                    TryBuildRefinery();
                
                }
            }
        }
        else {
            //No more command centers we desperately need to build one.
            TryBuildCommandCenter();
        }
    }
    
       
private:
  
    BotFunctions* botFunctions = new BotFunctions();
    const ObservationInterface* observation = Observation();
    ActionInterface* actions = Actions();
    std::vector<Point2D> bases;
    Point2D buildingPoint;

    bool TryBuildSupplyDepot()
    {
        
        /*// If we are not supply capped, don't build a supply depot.
        if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
            return false;
        }*/


        // Try and build a depot. Find a random SCV and give it the order.
        return botFunctions->TryBuildStructureFromPoint(ABILITY_ID::BUILD_SUPPLYDEPOT, buildingPoint);
    }

    bool TryBuildBarracks() {
        

        if (botFunctions->CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_SUPPLYDEPOT,buildingPoint) < 2)
        {
            return false;
        }

        if (botFunctions->CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_BARRACKS,buildingPoint) > 8)
        {
            return false;
        }

        return botFunctions->TryBuildStructureFromPoint(ABILITY_ID::BUILD_BARRACKS, buildingPoint);
    }

    bool TryBuildCommandCenter() {
        

        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5)
        {
            return false;
        }

        if (botFunctions->CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_COMMANDCENTER,buildingPoint) > 2) {
            return false;
        }

        //Build at random point close or somewhat far from base
        float rx = GetRandomScalar();
        float ry = GetRandomScalar();

        return botFunctions->TryBuildStructureAtPoint(ABILITY_ID::BUILD_COMMANDCENTER, Point2D(buildingPoint.x + rx * 45.0f, buildingPoint.y + ry * 45.0f));
    }


    bool TryBuildEngineeringBay() {

        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 6)
        {
            return false;
        }

        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
            return false;
        }
        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) > 2) {
            return false;
        }
        return botFunctions->TryBuildStructureFromPoint(ABILITY_ID::BUILD_ENGINEERINGBAY, bases[0]);
    }

    bool TryBuildTurrets() {
        
        //Need an engineering bay to build turrets
        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1)
        {
            return false;
        }
        if (botFunctions->CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_MISSILETURRET,buildingPoint) > 30) {
            return false;
        }

        //Build at random point close or somewhat far from base
        float rx = GetRandomScalar();
        float ry = GetRandomScalar();

        return botFunctions->TryBuildStructureAtPoint(ABILITY_ID::BUILD_MISSILETURRET, Point2D(buildingPoint.x + rx * 45.0f, buildingPoint.y + ry * 45.0f));
    }

    bool TryBuildRefinery() {

        
        
        if (botFunctions->CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_REFINERY, buildingPoint) >= 2) {
            return false;
        }

        //Need to find position of vespene gas
        const Unit* mineral_target = botFunctions->FindBestNearestUnit(buildingPoint, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
        if (!mineral_target) {
            return false;
        }

        const Unit* unit_to_build = nullptr;
        Units scvUnit = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
        for (const auto& unit : scvUnit)
        {
            for (const auto& order : unit->orders)
            {
                if (order.ability_id == ABILITY_ID::BUILD_REFINERY)
                {
                    return false;
                }
            }
            unit_to_build = unit;

        }

        actions->UnitCommand(unit_to_build,
            ABILITY_ID::BUILD_REFINERY,
            mineral_target);
        return true;
    }

    bool TryBuildFactory() {

        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 3) {
            return false;
        }
        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > 3) {
            return false;
        }

        return botFunctions->TryBuildStructureFromPoint(ABILITY_ID::BUILD_FACTORY, bases[0]);
    }

    bool TryBuildArmory() {

        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
            return false;
        }
        if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) >= 2) {
            return false;
        }
        return botFunctions->TryBuildStructureFromPoint(ABILITY_ID::BUILD_ARMORY, bases[0]);
    }
    
  
};

#endif
