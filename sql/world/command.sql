DELETE FROM `command` WHERE `name` IN ('chatcensure', 'chatcensure add', 'chatcensure delete', 'chatcensure reload');

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('chatcensure', 1, 'Syntax: .chatcensure $subcommand\nType .chatcensure to see the list of all available commands.'),
('chatcensure add', 1, 'Syntax: .chatcensure add <name>\nBan a word. Please use quotation marks when adding.'),
('chatcensure delete', 1, 'Syntax: .chatcensure delete <name>\nDelete a banned word. Please use quotation marks when deleting.'),
('chatcensure reload', 1, 'Syntax: .chatcensure reload\nRealod the chat Censure table.');
