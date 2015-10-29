PRAGMA journal_mode = PERSIST;

DROP TABLE IF EXISTS "InternationalDial";
CREATE TABLE InternationalDial (
	intl_type INTEGER,
	intl_name VARCHAR(64) PRIMARY KEY,
	intl_prefix VARCHAR(12) NOT NULL
);

INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_JPN", "+81");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_USA", "+1");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_CHN", "+86");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_THA", "+66");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_PHL", "+63");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_TWN", "+886");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_KOR", "+82");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_FRA", "+33");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_HKG","+852");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_GBR", "+44");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_DEU", "+49");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_SGP", "+65");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_CAN", "+1");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_IDN", "+62");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_ITA", "+39");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_AUS", "+61");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_MYS", "+60");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_VNM", "+84");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_IND", "+91");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_NLD", "+31");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_ESP", "+34");
INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(1, "IDS_DOCOMO_WORDING_BRA", "+55");

INSERT INTO "InternationalDial" (intl_type, intl_name, intl_prefix) VALUES(2, "WORLD CALL", "009130010");
