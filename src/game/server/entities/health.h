#ifndef GAME_SERVER_ENTITIES_HEALTHS_H
#define GAME_SERVER_ENTITIES_HEALTHS_H

#include <game/server/entity.h>

class CLifeHealth : public CEntity
{
public:
	CLifeHealth(CGameWorld *pGameWorld, vec2 Pos, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	int m_Owner;
	
private:
	vec2 Lefpos;
	bool StopTickLef;
	bool Fistheart;
	int m_Lifetime;
};


#endif 
