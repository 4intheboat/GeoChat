CREATE DATABASE geochat;
USE geochat;

CREATE TABLE chat (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    name VARBINARY(512) NOT NULL,
    PRIMARY KEY (id)
) CHARACTER SET utf8mb4, ENGINE InnoDB;

CREATE TABLE `user` (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    self_chat_id BIGINT UNSIGNED NOT NULL,
    name VARBINARY(512) NOT NULL,
    password VARBINARY(512) NOT NULL,
    stpath VARBINARY(512) NOT NULL,
    heartbit TIMESTAMP NOT NULL,
    PRIMARY KEY (id)
) CHARACTER SET utf8mb4, ENGINE InnoDB;

CREATE TABLE chatuser (
    user_id BIGINT UNSIGNED NOT NULL,
    chat_id BIGINT UNSIGNED NOT NULL,
    KEY (user_id),
    KEY (chat_id),
    CONSTRAINT `cu_user_id` FOREIGN KEY (`user_id`) REFERENCES `user` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT `cu_chat_id` FOREIGN KEY (`chat_id`) REFERENCES `chat` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) CHARACTER SET utf8mb4, ENGINE InnoDB;

CREATE TABLE message (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    user_id BIGINT UNSIGNED NOT NULL,
    chat_id BIGINT UNSIGNED NOT NULL,
    flags TINYINT UNSIGNED NOT NULL,
    time TIMESTAMP NOT NULL,
    text VARBINARY(512) NOT NULL,
    PRIMARY KEY (id),
    KEY (user_id),
    KEY (chat_id),
    KEY (time),
    CONSTRAINT `cm_user_id` FOREIGN KEY (`user_id`) REFERENCES `user` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT `cm_chat_id` FOREIGN KEY (`chat_id`) REFERENCES `chat` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) CHARACTER SET utf8mb4, ENGINE InnoDB;

CREATE USER 'me' IDENTIFIED BY '123';

GRANT SELECT, INSERT, UPDATE, DELETE, DROP ON geochat.`user` TO 'me';
GRANT SELECT, INSERT, UPDATE, DELETE, DROP ON geochat.chat TO 'me';
GRANT SELECT, INSERT, UPDATE, DELETE, DROP ON geochat.chatuser TO 'me';
GRANT SELECT, INSERT, UPDATE, DELETE, DROP ON geochat.message TO 'me';
