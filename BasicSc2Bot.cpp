#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart()
{
	std::cout << "hello, World!" << std::endl;
	return;
}

//Print the results of the players once the game is over.
void BasicSc2Bot::OnGameOver() {
	std::vector<PlayerResult> res = Observation()->GetResults();
	for (auto r : res) {
		std::cout << r.player_id << r.result << std::endl;
	}
	std::cout << "Game Over" << std::endl;
}

void BasicSc2Bot::OnStep()
{
	//std::cout << Observation()->GetGameLoop() << std::endl;
	TryBuildSupplyDepot();
	TryBuildRefinery();
	TryBuildCommandCenter();
	TryBuildBarracks();
	TryBuildEngineeringBay();
	TryBuildTurrets();
	TryBuildFactory();
	TryBuildStarPort();
	TryBuildFusionCore();
	AttackEnemy();
	//fixBuildings();
	return;
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::TERRAN_COMMANDCENTER:
	{
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		break;
	}

	case UNIT_TYPEID::TERRAN_SCV:
	{

		const Unit* mineral_target = FindNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_REFINERY);
		if (!mineral_target) {
			mineral_target = FindNearestUnit(unit->pos, UNIT_TYPEID::NEUTRAL_MINERALFIELD);
			if (!mineral_target) {
				break;
			}
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}

	case UNIT_TYPEID::TERRAN_BARRACKS:
	{
		if (unit->add_on_tag == 0)
		{
			TryAddOnTechLabBarracks();
		}

		if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 20)
		{
			if (CountUnitType(UNIT_TYPEID::TERRAN_REAPER) <= 15)
			{
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER);
				break;
			}
		}

		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		break;
	}

	case UNIT_TYPEID::TERRAN_MARINE:
	{
		const ObservationInterface* observation = Observation();
		const GameInfo& game_info = observation->GetGameInfo();
		Point2D newPoint = observation->GetStartLocation();
		size_t numberMarines = CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
		float rx = GetRandomScalar();
		float ry = GetRandomScalar();
		//Send every 10th idle marine to a random enemy location so that we can spot enemy
		if ((numberMarines % 20) == 0) {
			randomMarineLocation = rand() % game_info.enemy_start_locations.size();
			newPoint = game_info.enemy_start_locations[randomMarineLocation];
		}//Send others close to base
		else {
			newPoint = Point2D(newPoint.x + rx * 15.0f, newPoint.y + ry * 15.0f);
		}
		//Only send marines out if we have a small army
		if (numberMarines > 20) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_REAPER:
	{

		//std::cout << "Reaper produced";
		break;
	}

	default:
	{
		break;
	}
	}
}

//private

bool BasicSc2Bot::AttackEnemy() {
	const ObservationInterface* observation = Observation();
	Units uData = observation->GetUnits(Unit::Alliance::Enemy);
	if (uData.size() > 0) {
		//Send all units to the enemy
		Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
		Units sending = observation->GetUnits(Unit::Alliance::Self, filter);
		if (sending.size() > 15) {
			Actions()->UnitCommand(sending, ABILITY_ID::ATTACK_ATTACK, uData[0]);
		}

		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		sending = observation->GetUnits(Unit::Alliance::Self, filter);
		if (sending.size() > 1) {
			Actions()->UnitCommand(sending, ABILITY_ID::ATTACK_ATTACK, uData[0]);
		}

	}


	return false;
}


size_t BasicSc2Bot::CountUnitType(UNIT_TYPEID unit_type)
{
	return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

bool BasicSc2Bot::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type)
{
	const ObservationInterface* observation = Observation();
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
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		Point2D(startPoint.x + rx * 15.0f, startPoint.y + ry * 15.0f));

	return true;
}

bool BasicSc2Bot::TryBuildStructureAtPoint(ABILITY_ID ability_type_for_structure, Point2D point, UNIT_TYPEID unit_type)
{
	const ObservationInterface* observation = Observation();
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

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		point);

	return true;
}

bool BasicSc2Bot::TryBuildStructureAtUnit(ABILITY_ID ability_type_for_structure, const Unit* target_unit, UNIT_TYPEID unit_type) {
	const ObservationInterface* observation = Observation();
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

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		target_unit);

	return true;
}

bool BasicSc2Bot::TryBuildSupplyDepot()
{
	const ObservationInterface* observation = Observation();
	// If we are not supply capped, don't build a supply depot.
	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
		return false;

	// Try and build a depot. Find a random SCV and give it the order.
	return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

//Finds nearest unit to give position, for example find nearest mineral field for a scv
const Unit* BasicSc2Bot::FindNearestUnit(const Point2D& start, UNIT_TYPEID unit_type)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
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

const Unit* BasicSc2Bot::FindNearestGeyser(const Point2D& start) {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(Unit::Alliance::Neutral);
	Filter refinery_filter = UnitIsRefinery;
	Units refineries = observation->GetUnits(Unit::Alliance::Self, refinery_filter);
	std::vector<Point2D> r_points;
	const Unit* target = nullptr;
	float distance = std::numeric_limits<float>::max();

	for (const auto& u : refineries) {
		r_points.push_back(u->pos);
	}

	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
			if (std::find(r_points.begin(), r_points.end(), u->pos) != r_points.end()) {
				continue;
			}
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}

	return target;
}

bool BasicSc2Bot::UnitIsRefinery(const Unit& u) {
	return u.unit_type == UNIT_TYPEID::TERRAN_REFINERY;
}

bool BasicSc2Bot::TryBuildBarracks() {
	const ObservationInterface* observation = Observation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 3)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 7)
	{
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool BasicSc2Bot::TryBuildCommandCenter() {
	const ObservationInterface* observation = Observation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 2) {
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER);
}


bool BasicSc2Bot::TryBuildEngineeringBay() {

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_ENGINEERINGBAY);
}

bool BasicSc2Bot::TryBuildTurrets() {
	const ObservationInterface* observation = Observation();
	//Need an engineering bay to build turrets
	if (CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1)
	{
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_MISSILETURRET) > 30) {
		return false;
	}

	//Build at random point close or somewhat far from base
	Point2D startPoint = observation->GetStartLocation();
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();

	return TryBuildStructureAtPoint(ABILITY_ID::BUILD_MISSILETURRET, Point2D(startPoint.x + rx * 45.0f, startPoint.y + ry * 45.0f));
}

bool BasicSc2Bot::fixBuildings() {

	const ObservationInterface* observation = Observation();
	observation->GetRawObservation();

	return false;
}

bool BasicSc2Bot::TryBuildFactory() {
	const ObservationInterface* observation = Observation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1)
	{
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_FACTORY);
}

bool BasicSc2Bot::BuildBunkerAtPoint(const Point2D& p) {
	const ObservationInterface* observation = Observation();
	if (!observation->IsPlacable(p)) {
		return false;
	}

	return TryBuildStructureAtPoint(ABILITY_ID::BUILD_BUNKER, p);
}



bool BasicSc2Bot::TryBuildStarPort() {
	const ObservationInterface* observation = Observation();
	if (CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1)
	{
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_STARPORT);
}

bool BasicSc2Bot::TryBuildFusionCore() {
	const ObservationInterface* observation = Observation();
	if (CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1)
	{
		return false;
	}

	return TryBuildStructure(ABILITY_ID::BUILD_FUSIONCORE);
}

bool BasicSc2Bot::TryBuildRefinery() {

	const ObservationInterface* observation = Observation();
	Point2D startPoint = observation->GetStartLocation();
	if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) > 3) {
		return false;
	}

	//Need to find position of vespene gas
	const Unit* mineral_target = FindNearestGeyser(startPoint);
	if (!mineral_target) {
		return false;
	}

	TryBuildStructureAtUnit(ABILITY_ID::BUILD_REFINERY, mineral_target);

	return true;
}

bool BasicSc2Bot::TryAddOnTechLabBarracks() {

	const ObservationInterface* observation = Observation();
	Point2D startPoint = observation->GetStartLocation();

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 2) {
		return false;
	}

	Filter terran_barracks = IsUnit(UNIT_TYPEID::TERRAN_BARRACKS);
	Units barracks = observation->GetUnits(terran_barracks);

	float rx = GetRandomScalar();
	float ry = GetRandomScalar();

	for (const auto& unit : barracks)
	{
		if (unit->build_progress != 1)
		{
			return false;
		}

		Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS, 
			Point2D(unit->pos.x + rx * 15.0f, unit->pos.y + ry * 15.0f));
	}
	return true;
}