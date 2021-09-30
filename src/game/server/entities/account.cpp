/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <string.h>
#include <fstream>
#include <engine/config.h>
#include <engine/shared/config.h>
#include "account.h"


CAccount::CAccount(CPlayer *pPlayer, CGameContext *pGameServer)
{
	m_pPlayer = pPlayer;
	m_pGameServer = pGameServer;
}

void CAccount::Login(char *Username, char *Password)
{
	char aBuf[125];
	if (m_pPlayer->m_AccData.m_UserID)
		return GameServer()->SendChatTarget(m_pPlayer->GetCID(), "您已经登录了.");

	if (!Exists(Username))
		return GameServer()->SendChatTarget(m_pPlayer->GetCID(), "此账号不存在. 输入 /help");

	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);

	char AccUsername[32], AccPassword[32];
	int AccID;

	FILE *Accfile;
	Accfile = fopen(aBuf, "r");
	fscanf(Accfile, "%d\n%s\n%s", &AccID, AccUsername, AccPassword);
	fclose(Accfile);

	for (int j = 0; j < MAX_CLIENTS; j++)
	{
		if (GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->m_AccData.m_UserID == AccID)
		{
			dbg_msg("account", "账号登录失败 (账号 - '%s' - 正在被使用 (当前服务器))", Username);
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "账号正在被使用");
			return;
		}

		if (!GameServer()->m_aaExtIDs[j])
			continue;

		if (AccID == GameServer()->m_aaExtIDs[j])
		{
			dbg_msg("account", "账号登录失败 (账号 - '%s' - 正在被使用 (其他服务器))", Username);
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
			return;
		}
	}
	if (strcmp(Username, AccUsername) || strcmp(Password, AccPassword))
	{
		dbg_msg("account", "账号登录失败 (账号 - '%s' - 错误的名称)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "用户名/账号名错误");
		return;
	}

	Accfile = fopen(aBuf, "r");

	fscanf(Accfile, "%d\n%s\n%s\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d",
		&m_pPlayer->m_AccData.m_UserID,
		m_pPlayer->m_AccData.m_Username,
		m_pPlayer->m_AccData.m_Password,

		&m_pPlayer->m_AccData.m_Exp,
		&m_pPlayer->m_AccData.m_Level,
		&m_pPlayer->m_AccData.m_Money,

		&m_pPlayer->m_AccData.m_Dmg,
		&m_pPlayer->m_AccData.m_Health,
		&m_pPlayer->m_AccData.m_Ammoregen,
		&m_pPlayer->m_AccData.m_Handle,
		&m_pPlayer->m_AccData.m_Ammo,

		&m_pPlayer->m_AccData.m_PlayerState,

		&m_pPlayer->m_AccData.m_TurretMoney,
		&m_pPlayer->m_AccData.m_TurretLevel,
		&m_pPlayer->m_AccData.m_TurretExp,
		&m_pPlayer->m_AccData.m_TurretDmg,
		&m_pPlayer->m_AccData.m_TurretSpeed,
		&m_pPlayer->m_AccData.m_TurretAmmo,
		&m_pPlayer->m_AccData.m_TurretShotgun,
		&m_pPlayer->m_AccData.m_TurretRange,
		&m_pPlayer->m_AccData.m_Freeze,
		&m_pPlayer->m_AccData.m_Winner,
		&m_pPlayer->m_AccData.m_Luser);

	fclose(Accfile);

	if (m_pPlayer->GetTeam() == TEAM_SPECTATORS)
	{	
		if (GameServer()->m_pController->ZombStarted() && !GameServer()->m_pController->m_Warmup)
			m_pPlayer->SetTeam(TEAM_RED);
		else
			m_pPlayer->SetTeam(TEAM_BLUE);
	}

	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "登录成功！祝您玩的开心！");
}

void CAccount::Register(char *Username, char *Password)
{
	char aBuf[125];
	if(m_pPlayer->m_AccData.m_UserID)
		return GameServer()->SendChatTarget(m_pPlayer->GetCID(), "已经登录了");
	
	if(Exists(Username))
		return GameServer()->SendChatTarget(m_pPlayer->GetCID(), "账号已存在.");

	char Filter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";
	char *p = strpbrk(Username, Filter);
	if(!p)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "请使用以下字符注册用户名！");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}
	
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);

	FILE *Accfile;
	Accfile = fopen(aBuf, "a+");

	str_format(aBuf, sizeof(aBuf), "%d\n%s\n%s\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d", 
		NextID(),
		Username, 
		Password, 

		m_pPlayer->m_AccData.m_Exp,
		m_pPlayer->m_AccData.m_Level = g_Config.m_SvRegStartLevel,
		m_pPlayer->m_AccData.m_Money = g_Config.m_SvRegStartLevel,
		
		m_pPlayer->m_AccData.m_Dmg,
		m_pPlayer->m_AccData.m_Health,
		m_pPlayer->m_AccData.m_Ammoregen,
		m_pPlayer->m_AccData.m_Handle,
		m_pPlayer->m_AccData.m_Ammo,

		m_pPlayer->m_AccData.m_PlayerState,

		m_pPlayer->m_AccData.m_TurretMoney,
		m_pPlayer->m_AccData.m_TurretLevel,
		m_pPlayer->m_AccData.m_TurretExp,
		m_pPlayer->m_AccData.m_TurretDmg,
		m_pPlayer->m_AccData.m_TurretSpeed,
		m_pPlayer->m_AccData.m_TurretAmmo,
		m_pPlayer->m_AccData.m_TurretShotgun,
		m_pPlayer->m_AccData.m_TurretRange,
		m_pPlayer->m_AccData.m_Freeze,
		m_pPlayer->m_AccData.m_Winner,
		m_pPlayer->m_AccData.m_Luser);

	fputs(aBuf, Accfile);
	fclose(Accfile);

	Login(Username, Password);
	
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "~~~~~~~~~~~ ! 注册 ! ~~~~~~~~~~~");
	str_format(aBuf, sizeof(aBuf), "登录: %s", Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
	str_format(aBuf, sizeof(aBuf), "密码: %s", Password);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
	str_format(aBuf, sizeof(aBuf), "现在输入 /login %s %s", Username, Password);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "~~~~~~~~~~~ ! 注册 ! ~~~~~~~~~~~");
}

bool CAccount::Exists(const char *Username)
{
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);
    if(FILE *Accfile = fopen(aBuf, "r"))
    {
        fclose(Accfile);
        return true;
    }
    return false;
}

void CAccount::Apply()
{
	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", m_pPlayer->m_AccData.m_Username);
	std::remove(aBuf);
	FILE *Accfile;
	Accfile = fopen(aBuf,"a+");
	
	str_format(aBuf, sizeof(aBuf), "%d\n%s\n%s\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n\n%d\n%d", 
		
		m_pPlayer->m_AccData.m_UserID,
		m_pPlayer->m_AccData.m_Username,
		m_pPlayer->m_AccData.m_Password, 
		
		m_pPlayer->m_AccData.m_Exp,
		m_pPlayer->m_AccData.m_Level,
		m_pPlayer->m_AccData.m_Money,
		
		m_pPlayer->m_AccData.m_Dmg,
		m_pPlayer->m_AccData.m_Health,
		m_pPlayer->m_AccData.m_Ammoregen,
		m_pPlayer->m_AccData.m_Handle,
		m_pPlayer->m_AccData.m_Ammo,

		m_pPlayer->m_AccData.m_PlayerState,

		m_pPlayer->m_AccData.m_TurretMoney,
		m_pPlayer->m_AccData.m_TurretLevel,
		m_pPlayer->m_AccData.m_TurretExp,
		m_pPlayer->m_AccData.m_TurretDmg,
		m_pPlayer->m_AccData.m_TurretSpeed,
		m_pPlayer->m_AccData.m_TurretAmmo,
		m_pPlayer->m_AccData.m_TurretShotgun,
		m_pPlayer->m_AccData.m_TurretRange,
		m_pPlayer->m_AccData.m_Freeze,
		m_pPlayer->m_AccData.m_Winner,
		m_pPlayer->m_AccData.m_Luser);

	fputs(aBuf, Accfile);
	fclose(Accfile);
}

void CAccount::Reset()
{
	m_pPlayer->m_AccData.m_UserID = 0;
	str_copy(m_pPlayer->m_AccData.m_Username, "", 32);
	str_copy(m_pPlayer->m_AccData.m_Password, "", 32);
	
	m_pPlayer->m_AccData.m_Exp = m_pPlayer->m_AccData.m_Level = m_pPlayer->m_AccData.m_Money = 0;
	m_pPlayer->m_AccData.m_Dmg = m_pPlayer->m_AccData.m_Health = m_pPlayer->m_AccData.m_Ammoregen = m_pPlayer->m_AccData.m_Handle = m_pPlayer->m_AccData.m_Ammo = 0;
	m_pPlayer->m_AccData.m_PlayerState = 0;
	m_pPlayer->m_AccData.m_TurretMoney = m_pPlayer->m_AccData.m_TurretLevel = m_pPlayer->m_AccData.m_TurretExp = m_pPlayer->m_AccData.m_TurretDmg = m_pPlayer->m_AccData.m_TurretSpeed = 0;
	m_pPlayer->m_AccData.m_TurretAmmo = m_pPlayer->m_AccData.m_TurretShotgun = m_pPlayer->m_AccData.m_TurretRange = m_pPlayer->m_AccData.m_Freeze = 0;
}

void CAccount::NewPassword(char *NewPassword)
{
	if(str_length(NewPassword) > 12 || str_length(NewPassword) < 4)
	{
		char aBuf[48];
		str_format(aBuf, sizeof(aBuf), "密码太%s!", str_length(NewPassword)<4?"长了":"短了");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	
	char Filter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";
	char *p = strpbrk(NewPassword, Filter);
	if(!p)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "请使用以下字符作为注册密码！");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}
	
	str_copy(m_pPlayer->m_AccData.m_Password, NewPassword, 32);
	Apply();
	
	dbg_msg("account", "密码更改为 - ('%s')", m_pPlayer->m_AccData.m_Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "密码更改成功");
}


int CAccount::NextID()
{
	FILE *Accfile;
	int UserID = 1;
	char aBuf[32], AccUserID[32];

	str_copy(AccUserID, "accounts/UsersID.acc", sizeof(AccUserID));

	if(Exists("UsersID"))
	{
		Accfile = fopen(AccUserID, "r");
		fscanf(Accfile, "%d", &UserID);
		fclose(Accfile);

		std::remove(AccUserID);

		Accfile = fopen(AccUserID, "a+");
		str_format(aBuf, sizeof(aBuf), "%d", UserID+1);
		fputs(aBuf, Accfile);
		fclose(Accfile);

		return UserID+1;
	}
	else
	{
		Accfile = fopen(AccUserID, "a+");
		str_format(aBuf, sizeof(aBuf), "%d", UserID);
		fputs(aBuf, Accfile);
		fclose(Accfile);
	}

	return 1;
}
