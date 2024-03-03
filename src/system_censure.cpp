#include "ScriptMgr.h"
#include "Chat.h"
#include <list>
#include "Containers.h"
#include "Player.h"
#include "Config.h"

std::vector<std::string> chat;

namespace
{
    bool _isEnableCC = false;
    bool _isAnnounceCC = false;
}

class SystemCensure : public PlayerScript
{
public:
    SystemCensure() : PlayerScript("SystemCensure") {}

    bool _isEnableCC = sConfigMgr->GetOption<bool>("ChatCensure.Enable", true);
    bool _isAnnounceCC = sConfigMgr->GetOption<bool>("ChatCensure.Announce", true);

    // Announce Module
    void OnLogin(Player* player) override
    {
        if (_isEnableCC)
        {
            if (_isAnnounceCC)
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Chat Censure |rmodule.");
            }
        }
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg) override
    {
        CheckMessage(player, msg, nullptr, nullptr, nullptr);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        CheckMessage(player, msg, receiver, nullptr, nullptr);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Group* group) override
    {
        CheckMessage(player, msg, nullptr, group, nullptr);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Guild* guild) override
    {
        CheckMessage(player, msg, nullptr, nullptr, guild);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Channel* /*channel*/) override
    {
        CheckMessage(player, msg, nullptr, nullptr, nullptr);
    }

    void CheckMessage(Player* player, std::string& msg, Player* /*receiver*/, Group* /*group*/, Guild* /*guild*/)
    {
        //if account is game master let them say what ever they like just incase they need to send the website
        if (player->GetSession()->GetSecurity() >= 1)
            return;

        // transform to lowercase (for simpler checking)
        std::string lower = msg;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        for (auto& i : chat)
        {
            if (lower.find(i) != std::string::npos)
            {
                msg = "";
                ChatHandler(player->GetSession()).PSendSysMessage("Word not allowed, take care of your vocabulary!");
                return;
            }
        }
    }
};

class LoadChatTable : public WorldScript
{
public:
    LoadChatTable() : WorldScript("load_system_censure") { }

    void OnLoadCustomDatabaseTable()
    {
        LOG_INFO("server.loading", "Loading Chat Censure...");
        QueryResult result = CharacterDatabase.Query("SELECT `text` FROM `chat_censure`");

        if (!result)
        {
            LOG_INFO("server.loading", ">> Loaded 0 Chat Censures. DB table `Chat_Censure` is empty!");
            LOG_INFO("server.loading", " ");
            return;
        }

        uint32 count = 0;
        uint32 oldMSTime = getMSTime();

        do
        {
            Field* field = result->Fetch();
            chat.push_back(field[0].Get<std::string>());
            count++;
        }
        while (result->NextRow());

        LOG_INFO("server.loading", ">> Loaded {} chat_censure in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
        LOG_INFO("server.loading", " ");
    }
};

using namespace Acore::ChatCommands;

class ChatCensureCommand : public CommandScript
{
public:
    ChatCensureCommand() : CommandScript("ChatCensureCommand") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable ChatCensureCommandTable =
        {
            { "reload", HandleReloadCommand, SEC_GAMEMASTER, Console::No },
            { "add", HandleAddCommand, SEC_GAMEMASTER, Console::No },
            { "delete", HandleDeleteCommand, SEC_GAMEMASTER, Console::No },
        };

        static ChatCommandTable ChatCensurePBaseTable =
        {
            { "chatcensure", ChatCensureCommandTable }
        };

        return ChatCensurePBaseTable;
    }

    static bool HandleReloadCommand(ChatHandler* handler)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!me)
            return false;

        chat.clear();
        QueryResult result = CharacterDatabase.Query("SELECT `id`,`text` FROM `chat_censure`");
        uint32 count = 0;
        uint32 oldMSTime = getMSTime();

        do
        {
            Field* field = result->Fetch();
            uint8 id = field[0].Get<uint8>();
            chat.push_back(field[1].Get<std::string>());
            count++;
        }
        while (result->NextRow());

        ChatHandler(me->GetSession()).PSendSysMessage("Reloaded %u chat censorship in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        return true;
    }

    static bool HandleAddCommand(ChatHandler* handler, std::string args)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!args.empty())
        {
            handler->SendSysMessage(LANG_IMPROPER_VALUE);
            handler->SetSentErrorMessage(true);
            ChatHandler(me->GetSession()).PSendSysMessage("Write the word to censor.");
            return false;
        }

        //lets check the Database to see if arguement already exist
        QueryResult result = CharacterDatabase.Query("SELECT `text` FROM `chat_censure` WHERE `text` = '{}'", args);

        if (result)
        {
            ChatHandler(me->GetSession()).PSendSysMessage("The word already exists: |cff4CFF00 %s|r.", args);
        }
        else
        {
            CharacterDatabase.Query("INSERT INTO `chat_censure` (`text`) VALUES ('{}')", args);
            ChatHandler(me->GetSession()).PSendSysMessage("Added: |cff4CFF00 %s|r to chat censorship. Reload the table to activate it.", args);
        }
        return true;
    }

    static bool HandleDeleteCommand(ChatHandler* handler, std::string args)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!args.empty())
        {
            handler->SendSysMessage(LANG_IMPROPER_VALUE);
            handler->SetSentErrorMessage(true);
            ChatHandler(me->GetSession()).PSendSysMessage("Write the word to delete.");
            return false;
        }

        QueryResult result = CharacterDatabase.Query("SELECT `text` FROM `chat_censure` WHERE `text` = '{}'", args);

        if (!result)
        {
            ChatHandler(me->GetSession()).PSendSysMessage("The word : |cff4CFF00 %s|r cannot be found in the database.", args);
        }
        else
        {
            CharacterDatabase.Query("DELETE FROM `chat_censure` WHERE `text` = '{}'", args);
            ChatHandler(me->GetSession()).PSendSysMessage("Cleared: |cff4CFF00 %s|r Reload the table to activate it.", args);
        }
        return true;
    }
};

void AddSC_SystemCensure()
{
    new ChatCensureCommand();
    new SystemCensure();
    new LoadChatTable();
}
