DELETE FROM `command` WHERE `name` IN ('chatcensure');
DELETE FROM `command` WHERE `name` IN ('chatcensure add');
DELETE FROM `command` WHERE `name` IN ('chatcensure delete');
DELETE FROM `command` WHERE `name` IN ('chatcensure reload');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('chatcensure', 1, 'Syntax: .chatcensure $subcommand\nType .chatcensure to see the list of all available commands.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('chatcensure add', 1, 'Syntax: .chatcensure add <name>\nBan a word. Please use quotation marks when adding.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('chatcensure delete', 1, 'Syntax: .chatcensure delete <name>\nDelete a banned word. Please use quotation marks when deleting.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('chatcensure reload', 1, 'Syntax: .chatcensure reload\nRealod the chat Censure table.');
