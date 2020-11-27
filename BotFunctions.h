#ifndef Bot_Functions_
#define Bot_Functions_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>
#include "Builder.h"

using namespace sc2;

class BotFunctions : public sc2::Agent {
 public:

        void setVars(const ObservationInterface* obs, ActionInterface* act) {
            observation = obs;
            actions = act;
        }

        size_t CountUnitType(UNIT_TYPEID unit_type)
        {
            return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
        }

        size_t CountUnitTypeFromPoint(UNIT_TYPEID unit_type, Point2D startPoint)
        {
            size_t number = 0;
            Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
            for (auto u : units) {
                if (Distance2D(u->pos, startPoint) < 100) {
                    ++number;
                }
            }
            return number;
        }
       
        bool TryBuildStructureFromPoint(ABILITY_ID ability_type_for_structure, Point2D startPoint, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV)
        {

            // If a unit already is building a supply structure of this type, do nothing.
            // Also get an scv to build the structure.
            const Unit* unit_to_build = nullptr;
            Units units = observation->GetUnits(Unit::Alliance::Self);
            for (const auto& unit : units)
            {
                for (const auto& order : unit->orders)
                {
                    if (order.ability_id == ability_type_for_structure)
                    {
                        return false;
                    }
                }
                if (unit->unit_type == unit_type)
                {
                    unit_to_build = unit;
                }
            }
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();

            actions->UnitCommand(unit_to_build,
                ability_type_for_structure,
                Point2D(startPoint.x + rx * 15.0f, startPoint.y + ry * 15.0f));

            return true;
        }

        bool TryBuildStructureAtPoint(ABILITY_ID ability_type_for_structure, Point2D point, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV)
        {

            // If a unit already is building a supply structure of this type, do nothing.
            // Also get an scv to build the structure.
            const Unit* unit_to_build = nullptr;
            Units units = observation->GetUnits(Unit::Alliance::Self);
            Point2D startPoint = observation->GetStartLocation();
            for (const auto& unit : units)
            {
                for (const auto& order : unit->orders)
                {
                    if (order.ability_id == ability_type_for_structure)
                    {
                        return false;
                    }
                }
                if (unit->unit_type == unit_type)
                {
                    unit_to_build = unit;
                }
            }

            actions->UnitCommand(unit_to_build,
                ability_type_for_structure,
                point);

            return true;
        }

        //Finds nearest unit from given position, for example find nearest mineral field for a scv
        const Unit* FindNearestUnit(const Point2D& start, UNIT_TYPEID unit_type, int alliance = 3)
        {

            //Filter filter = IsUnit(unit_type);
            Units units;

            if (alliance == 1) {
                units = observation->GetUnits(Unit::Alliance::Self);
            }
            else if (alliance == 3) {
                units = observation->GetUnits(Unit::Alliance::Neutral);
            }
            else {
                units = observation->GetUnits();
            }
            float distance = std::numeric_limits<float>::max();
            const Unit* target = nullptr;

            for (const auto& u : units)
            {
                if (u->unit_type == unit_type)
                {
                    float d = DistanceSquared2D(u->pos, start);
                    if (d < distance)
                    {
                        distance = d;
                        target = u;
                    }
                }
            }

            return target;
        }

        //Finds nearest unit from given position, for example find nearest mineral field for a scv
        const Unit* FindBestNearestUnit(const Point2D& start, UNIT_TYPEID unit_type, int alliance = 3)
        {

            //Filter filter = IsUnit(unit_type);
            Units units;

            if (alliance == 1) {
                units = observation->GetUnits(Unit::Alliance::Self);
            }
            else if (alliance == 3) {
                units = observation->GetUnits(Unit::Alliance::Neutral);
            }
            else {
                units = observation->GetUnits();
            }
            float distance = std::numeric_limits<float>::max();
            const Unit* target = nullptr;

            for (const auto& u : units)
            {
                if (u->unit_type == unit_type)
                {
                    //If refinery is full, find another one
                    if (unit_type == UNIT_TYPEID::TERRAN_REFINERY) {
                        if (u->assigned_harvesters >= 3) {
                            continue;
                        }
                        else if (u->vespene_contents <= 0){
                            continue;
                        }
                    }
                
                    if (unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
                        if (u->assigned_harvesters >= 16) {
                            continue;
                        }
                    }

                    if (unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
                        //When trying to build on a geyser find out if a refinrey is already build on it by getting the distance to refineries
                        Units refineries = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
                        bool already = false;
                        for (auto r : refineries) {
                            if (DistanceSquared2D(u->pos, r->pos) < 5.0) {
                                already = true;
                                break;
                            }
                        }
                        if (already) {
                            continue;
                        }
                    }


                    float d = DistanceSquared2D(u->pos, start);
                    if (d < distance)
                    {
                        distance = d;
                        target = u;
                    }
                }
            }

            return target;
        }

 private:
     const ObservationInterface* observation = Observation();
     ActionInterface* actions = Actions();
};
#endif
