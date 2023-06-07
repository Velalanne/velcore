/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: port_commandscript
%Complete: 100
Comment: All parcels related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Group.h"
#include "Player.h"
#include "Realm.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "MiscPackets.h"
#include "DatabaseEnv.h"

using namespace Trinity::ChatCommands;

class port_commandscript : public CommandScript
{
public:
    port_commandscript() : CommandScript("port_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
        {
            { "port add",   HandlePortAddCommand,  rbac::RBAC_ROLE_ADMINISTRATOR,    Console::No },
        };
        return commandTable;
    }

    static bool PrintPlayerSpawnMessage(ChatHandler* handler, Player* player, std::string message)
    {
        handler->PSendSysMessage("|cffFFFFFF[|cffFFFF00Vel's|cffFFFFFF]|cffFFFFFF[|cff00fff4Porty|cffFFFFFF]: |cffFFFFFF%s|r", message);
        return true;
    }

    static bool HandlePortAddCommand(ChatHandler* handler, uint32 displayid, float scale, Tail name)
    {
        if (name.empty())
            return false;

        Player* player = handler->GetPlayer();

        uint32 entry = 0;

        std::string newName = "";
        newName += name;

        uint32 newGobId = 0;
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_GAMEOBJECT_TEMPLATE_MAX_ID);
        PreparedQueryResult result = WorldDatabase.Query(stmt);

        uint32 maxGobId = result->Fetch()->GetInt32();
        newGobId = maxGobId+1;

        stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GAMEOBJECT_TEMPLATE);
        stmt->setUInt32(0, newGobId);
        stmt->setUInt32(1, 2);
        stmt->setUInt32(2, displayid);
        stmt->setString(3, "[PORT]: " + newName);
        stmt->setFloat(4, scale);
        WorldDatabase.Execute(stmt);

        stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_SMART_SCRIPTS);
        stmt->setUInt32(0, newGobId);
        stmt->setUInt32(1, 1);
        WorldDatabase.Execute(stmt);

        stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_SMART_SCRIPTS);
        stmt->setUInt32(0, newGobId);
        stmt->setUInt32(1, 1);
        stmt->setUInt32(2, 64);
        stmt->setUInt32(3, 100);
        stmt->setUInt32(4, 62);
        stmt->setUInt32(5, player->GetMapId());
        stmt->setUInt32(6, 7);
        stmt->setFloat(7, player->GetPositionX());
        stmt->setFloat(8, player->GetPositionY());
        stmt->setFloat(9, player->GetPositionZ());
        stmt->setFloat(10, player->GetOrientation());
        stmt->setString(11, "Auto generated teleporter AI ["+newName+"]");
        WorldDatabase.Execute(stmt);

        PrintPlayerSpawnMessage(handler, player, "Teleport ([PORT]: " + newName + ") vytvořen! Změny se projeví po restartu.");

        return true;
    }
};

void AddSC_port_commandscript()
{
    new port_commandscript();
}
