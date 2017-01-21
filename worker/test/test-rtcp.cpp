#include "include/catch.hpp"
#include "include/helpers.h"
#include "common.h"
#include "RTC/RTCP/Packet.h"
#include "RTC/RTCP/Sdes.h"
#include "RTC/RTCP/SenderReport.h"
#include "RTC/RTCP/ReceiverReport.h"
#include "RTC/RTCP/Bye.h"
#include "RTC/RTCP/FeedbackRtpNack.h"
#include "RTC/RTCP/FeedbackRtpTmmb.h"
#include "RTC/RTCP/FeedbackRtpTllei.h"
#include "RTC/RTCP/FeedbackRtpEcn.h"
#include "RTC/RTCP/FeedbackPsSli.h"
#include "RTC/RTCP/FeedbackPsRpsi.h"
#include "RTC/RTCP/FeedbackPsFir.h"
#include "RTC/RTCP/FeedbackPsTst.h"
#include "RTC/RTCP/FeedbackPsVbcm.h"
#include "RTC/RTCP/FeedbackPsLei.h"
#include "RTC/RTCP/FeedbackPsAfb.h"
#include <string>

using namespace RTC::RTCP;

SCENARIO("parse RTCP packets", "[parser][rtcp]")
{
	SECTION("minimum header")
	{
		uint8_t buffer[] =
		{
			0x81, 0xca, 0x00, 0x00 // RTCP common header
		};

		Packet* packet = Packet::Parse(buffer, sizeof(buffer));

		REQUIRE(packet);

		delete packet;
	}

	SECTION("buffer is to small")
	{
		uint8_t buffer[] =
		{
			0x81, 0xca, 0x00
		};

		Packet* packet = Packet::Parse(buffer, sizeof(buffer));

		REQUIRE_FALSE(packet);

		delete packet;
	}

	SECTION("version is zero")
	{
		uint8_t buffer[] =
		{
			0x00, 0xca, 0x00, 0x01,
			0x00, 0x00, 0x00, 0x00
		};

		Packet* packet = Packet::Parse(buffer, sizeof(buffer));

		REQUIRE_FALSE(packet);

		delete packet;
	}

	SECTION("length is wrong")
	{
		uint8_t buffer[] =
		{
			0x81, 0xca, 0x00, 0x04,
			0x00, 0x00, 0x00, 0x00
		};

		Packet* packet = Packet::Parse(buffer, sizeof(buffer));

		REQUIRE_FALSE(packet);

		delete packet;
	}

	SECTION("type is unknown")
	{
		uint8_t buffer[] =
		{
			0x81, 0x00, 0x00, 0x01,
			0x00, 0x00, 0x00, 0x00
		};

		Packet* packet = Packet::Parse(buffer, sizeof(buffer));

		REQUIRE_FALSE(packet);

		delete packet;
	}

	SECTION("parse SdesChunk")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x00, // SDES SSRC
			0x01, 0x0a, 0x6f, 0x75, // SDES Item
			0x74, 0x43, 0x68, 0x61,
			0x6e, 0x6e, 0x65, 0x6c
		};

		uint32_t ssrc = 0;
		SdesItem::Type type = SdesItem::Type::CNAME;
		std::string value = "outChannel";
		size_t len = value.size();

		SdesChunk* chunk = SdesChunk::Parse(buffer, sizeof(buffer));

		REQUIRE(chunk->GetSsrc() == ssrc);

		SdesItem* item = *(chunk->Begin());

		REQUIRE(item->GetType() == type);
		REQUIRE(item->GetLength() == len);
		REQUIRE(std::string(item->GetValue(), len) == "outChannel");

		delete chunk;
	}

	SECTION("create SdesChunk")
	{
		uint32_t ssrc = 0;
		SdesItem::Type type = SdesItem::Type::CNAME;
		std::string value = "outChannel";
		size_t len = value.size();

		// Create sdes item.
		SdesItem* item = new SdesItem(type, len, value.c_str());

		// // Create sdes chunk.
		SdesChunk chunk(ssrc);
		chunk.AddItem(item);

		// // Check chunk content.
		REQUIRE(chunk.GetSsrc() == ssrc);

		// // Check item content.
		item = *(chunk.Begin());

		REQUIRE(item->GetType() == type);
		REQUIRE(item->GetLength() == len);
		REQUIRE(std::string(item->GetValue(), len) == "outChannel");
	}

	SECTION("parse SenderReport")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x04, 0xD2, // ssrc
			0x00, 0x00, 0x04, 0xD2, // ntp sec
			0x00, 0x00, 0x04, 0xD2, // ntp frac
			0x00, 0x00, 0x04, 0xD2, // rtp ts
			0x00, 0x00, 0x04, 0xD2, // packet count
			0x00, 0x00, 0x04, 0xD2, // octet count
		};

		uint32_t ssrc = 1234;
		uint32_t ntpSec = 1234;
		uint32_t ntpFrac = 1234;
		uint32_t rtpTs = 1234;
		uint32_t packetCount = 1234;
		uint32_t octetCount = 1234;

		SenderReport* report = SenderReport::Parse(buffer, sizeof(SenderReport::Header));

		REQUIRE(report);

		REQUIRE(report->GetSsrc() == ssrc);
		REQUIRE(report->GetNtpSec() == ntpSec);
		REQUIRE(report->GetNtpFrac() == ntpFrac);
		REQUIRE(report->GetRtpTs() == rtpTs);
		REQUIRE(report->GetPacketCount() == packetCount);
		REQUIRE(report->GetOctetCount() == octetCount);

		delete report;
	}

	SECTION("create SenderReport")
	{
		uint32_t ssrc = 1234;
		uint32_t ntpSec = 1234;
		uint32_t ntpFrac = 1234;
		uint32_t rtpTs = 1234;
		uint32_t packetCount = 1234;
		uint32_t octetCount = 1234;

		// Create local report and check content.
		// SenderReport();
		SenderReport report1;

		report1.SetSsrc(ssrc);
		report1.SetNtpSec(ntpSec);
		report1.SetNtpFrac(ntpFrac);
		report1.SetRtpTs(rtpTs);
		report1.SetPacketCount(packetCount);
		report1.SetOctetCount(octetCount);

		REQUIRE(report1.GetSsrc() == ssrc);
		REQUIRE(report1.GetNtpSec() == ntpSec);
		REQUIRE(report1.GetNtpFrac() == ntpFrac);
		REQUIRE(report1.GetRtpTs() == rtpTs);
		REQUIRE(report1.GetPacketCount() == packetCount);
		REQUIRE(report1.GetOctetCount() == octetCount);

		// Create report out of the existing one and check content.
		// SenderReport(SenderReport* report);
		SenderReport report2(&report1);

		REQUIRE(report2.GetSsrc() == ssrc);
		REQUIRE(report2.GetNtpSec() == ntpSec);
		REQUIRE(report2.GetNtpFrac() == ntpFrac);
		REQUIRE(report2.GetRtpTs() == rtpTs);
		REQUIRE(report2.GetPacketCount() == packetCount);
		REQUIRE(report2.GetOctetCount() == octetCount);

		// Locally store the content of the report.
		report2.Serialize();

		// Create report out of buffer and check content.
		// SenderReport(Header* header);
		SenderReport report3((SenderReport::Header*)report2.GetRaw());

		REQUIRE(report3.GetSsrc() == ssrc);
		REQUIRE(report3.GetNtpSec() == ntpSec);
		REQUIRE(report3.GetNtpFrac() == ntpFrac);
		REQUIRE(report3.GetRtpTs() == rtpTs);
		REQUIRE(report3.GetPacketCount() == packetCount);
		REQUIRE(report3.GetOctetCount() == octetCount);
	}

	SECTION("parse ReceiverReport")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x04, 0xD2,	// ssrc
			0x01,                   // fraction_lost
			0x00, 0x00, 0x04,       // total_lost
			0x00, 0x00, 0x04, 0xD2,	// last_seq
			0x00, 0x00, 0x04, 0xD2,	// jitter
			0x00, 0x00, 0x04, 0xD2,	// lsr
			0x00, 0x00, 0x04, 0xD2  // dlsr
		};

		uint32_t ssrc = 1234;
		uint8_t fractionLost = 1;
		int32_t totalLost = 4;
		uint32_t lastSeq = 1234;
		uint32_t jitter = 1234;
		uint32_t lastSenderReport = 1234;
		uint32_t delaySinceLastSenderReport = 1234;

		ReceiverReport* report = ReceiverReport::Parse(buffer, sizeof(ReceiverReport::Header));

		REQUIRE(report);

		REQUIRE(report->GetSsrc() == ssrc);
		REQUIRE(report->GetFractionLost() == fractionLost);
		REQUIRE(report->GetTotalLost() == totalLost);
		REQUIRE(report->GetLastSeq() == lastSeq);
		REQUIRE(report->GetJitter() == jitter);
		REQUIRE(report->GetLastSenderReport() == lastSenderReport);
		REQUIRE(report->GetDelaySinceLastSenderReport() == delaySinceLastSenderReport);
	}

	SECTION("create ReceiverReport")
	{
		uint32_t ssrc = 1234;
		uint8_t fractionLost = 1;
		int32_t totalLost = 4;
		uint32_t lastSeq = 1234;
		uint32_t jitter = 1234;
		uint32_t lastSenderReport = 1234;
		uint32_t delaySinceLastSenderReport = 1234;

		// Create local report and check content.
		// ReceiverReport();
		ReceiverReport report1;

		report1.SetSsrc(ssrc);
		report1.SetFractionLost(fractionLost);
		report1.SetTotalLost(totalLost);
		report1.SetLastSeq(lastSeq);
		report1.SetJitter(jitter);
		report1.SetLastSenderReport(lastSenderReport);
		report1.SetDelaySinceLastSenderReport(delaySinceLastSenderReport);

		REQUIRE(report1.GetSsrc() == ssrc);
		REQUIRE(report1.GetFractionLost() == fractionLost);
		REQUIRE(report1.GetTotalLost() == totalLost);
		REQUIRE(report1.GetLastSeq() == lastSeq);
		REQUIRE(report1.GetJitter() == jitter);
		REQUIRE(report1.GetLastSenderReport() == lastSenderReport);
		REQUIRE(report1.GetDelaySinceLastSenderReport() == delaySinceLastSenderReport);

		// Create report out of the existing one and check content.
		// ReceiverReport(ReceiverReport* report);
		ReceiverReport report2(&report1);

		REQUIRE(report2.GetSsrc() == ssrc);
		REQUIRE(report2.GetFractionLost() == fractionLost);
		REQUIRE(report2.GetTotalLost() == totalLost);
		REQUIRE(report2.GetLastSeq() == lastSeq);
		REQUIRE(report2.GetJitter() == jitter);
		REQUIRE(report2.GetLastSenderReport() == lastSenderReport);
		REQUIRE(report2.GetDelaySinceLastSenderReport() == delaySinceLastSenderReport);

		// Locally store the content of the report.
		report2.Serialize();

		// Create report out of buffer and check content.
		// ReceiverReport(Header* header);
		ReceiverReport report3((ReceiverReport::Header*)report2.GetRaw());

		REQUIRE(report2.GetSsrc() == ssrc);
		REQUIRE(report2.GetFractionLost() == fractionLost);
		REQUIRE(report2.GetTotalLost() == totalLost);
		REQUIRE(report2.GetLastSeq() == lastSeq);
		REQUIRE(report2.GetJitter() == jitter);
		REQUIRE(report2.GetLastSenderReport() == lastSenderReport);
		REQUIRE(report2.GetDelaySinceLastSenderReport() == delaySinceLastSenderReport);
	}

	SECTION("create ByePacket")
	{
		uint32_t ssrc1 = 1111;
		uint32_t ssrc2 = 2222;
		std::string reason("hasta la vista");

		// Create local Bye packet and check content.
		// ByePacket();
		ByePacket bye1;

		bye1.AddSsrc(ssrc1);
		bye1.AddSsrc(ssrc2);
		bye1.SetReason(reason);

		ByePacket::Iterator it = bye1.Begin();

		REQUIRE(*it == ssrc1);
		it++;
		REQUIRE(*it == ssrc2);
		REQUIRE(bye1.GetReason() == reason);

		// Locally store the content of the packet.
		uint8_t buffer[bye1.GetSize()];
		bye1.Serialize(buffer);

		// Parse the buffer of the previous packet and check content.
		ByePacket* bye2 = ByePacket::Parse(buffer, sizeof(buffer));

		it = bye2->Begin();

		REQUIRE(*it == ssrc1);
		it++;
		REQUIRE(*it == ssrc2);
		REQUIRE(bye2->GetReason() == reason);
	}

	SECTION("parse NackItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x01, 0x02, 0x00
		};

		uint16_t packetId = 1;
		uint16_t lostPacketBitmask = 2;

		NackItem* item = NackItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetPacketId() == packetId);
		REQUIRE(item->GetLostPacketBitmask() == lostPacketBitmask);

		delete item;
	}

	SECTION("create NackItem")
	{
		uint16_t packetId = 1;
		uint16_t lostPacketBitmask = 2;

		// Create local NackItem and check content.
		// NackItem();
		NackItem item1(packetId, lostPacketBitmask);

		REQUIRE(item1.GetPacketId() == packetId);
		REQUIRE(item1.GetLostPacketBitmask() == htons(lostPacketBitmask));

		// Create local NackItem out of existing one and check content.
		// NackItem(NackItem*);
		NackItem item2(&item1);

		REQUIRE(item2.GetPacketId() == packetId);
		REQUIRE(item2.GetLostPacketBitmask() == htons(lostPacketBitmask));

		// Locally store the content of the packet.
		uint8_t buffer[item2.GetSize()];

		item2.Serialize(buffer);

		// Create local NackItem out of previous packet buffer and check content.
		// NackItem(NackItem::Header*);
		NackItem item3((NackItem::Header*)buffer);

		REQUIRE(item3.GetPacketId() == packetId);
		REQUIRE(item3.GetLostPacketBitmask() == htons(lostPacketBitmask));
	}

	SECTION("parse TmmbrItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x00, // ssrc
			0x04, 0x00, 0x02, 0x01
		};

		uint32_t ssrc = 0;
		uint64_t bitrate = 2;
		uint32_t overhead = 1;

		TmmbrItem* item = TmmbrItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSsrc() == ssrc);
		REQUIRE(item->GetBitrate() == bitrate);
		REQUIRE(item->GetOverhead() == overhead);

		delete item;
	}

	SECTION("parse TlleiItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x01, 0x02, 0x00
		};

		uint16_t packetId = 1;
		uint16_t lostPacketBitmask = 2;

		TlleiItem* item = TlleiItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetPacketId() == packetId);
		REQUIRE(item->GetLostPacketBitmask() == lostPacketBitmask);

		delete item;
	}

	SECTION("parse EcnItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x01, // Extended Highest Sequence Number
			0x00, 0x00, 0x00, 0x01, // ECT (0) Counter
			0x00, 0x00, 0x00, 0x01, // ECT (1) Counter
			0x00, 0x01,             // ECN-CE Counter
			0x00, 0x01,             // not-ECT Counter
			0x00, 0x01,             // Lost Packets Counter
			0x00, 0x01              // Duplication Counter
		};

		uint32_t sequenceNumber = 1;
		uint32_t ect0Counter = 1;
		uint32_t ect1Counter = 1;
		uint16_t ecnCeCounter = 1;
		uint16_t notEctCounter = 1;
		uint16_t lostPackets = 1;
		uint16_t duplicatedPackets = 1;

		EcnItem* item = EcnItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSequenceNumber() == sequenceNumber);
		REQUIRE(item->GetEct0Counter() == ect0Counter);
		REQUIRE(item->GetEct1Counter() == ect1Counter);
		REQUIRE(item->GetEcnCeCounter() == ecnCeCounter);
		REQUIRE(item->GetNotEctCounter() == notEctCounter);
		REQUIRE(item->GetLostPackets() == lostPackets);
		REQUIRE(item->GetDuplicatedPackets() == duplicatedPackets);

		delete item;
	}

	SECTION("parse SliItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x08, 0x01, 0x01
		};

		uint16_t first = 1;
		uint16_t number = 4;
		uint8_t pictureId = 1;

		SliItem* item = SliItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetFirst() == first);
		REQUIRE(item->GetNumber() == number);
		REQUIRE(item->GetPictureId() == pictureId);

		delete item;
	}

	SECTION("parse RpsiItem")
	{
		uint8_t buffer[] =
		{
			0x08,                   // Padding Bits
			0x02,                   // Zero | Payload Type
			0x00, 0x00,             // Native RPSI bit string
			0x00, 0x00, 0x01, 0x00
		};

		uint8_t payloadType = 1;
		uint8_t payloadMask = 1;
		size_t length = 5;

		RpsiItem* item = RpsiItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetPayloadType() == payloadType);
		REQUIRE(item->GetLength() == length);
		REQUIRE((item->GetBitString()[item->GetLength()-1] & 1) == payloadMask);

		delete item;
	}

	SECTION("parse FirItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x00, // SSRC
			0x08, 0x00, 0x00, 0x00 // Seq nr.
		};

		uint32_t ssrc = 0;
		uint8_t seq = 8;

		FirItem* item = FirItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSsrc() == ssrc);
		REQUIRE(item->GetSequenceNumber() == seq);

		delete item;
	}

	SECTION("parse TstnItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x00, // SSRC
			0x08,                   // Seq nr.
			0x00, 0x00, 0x08        // Reserved | Index
		};

		uint32_t ssrc = 0;
		uint8_t seq = 8;
		uint8_t index = 1;

		TstnItem* item = TstnItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSsrc() == ssrc);
		REQUIRE(item->GetSequenceNumber() == seq);
		REQUIRE(item->GetIndex() == index);

		delete item;
	}

	SECTION("parse VbcmItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x00, // SSRC
			0x08,                   // Seq nr.
			0x02,                   // Zero | Payload Vbcm
			0x00, 0x01,             // Length
			0x01,                   // VBCM Octet String
			0x00, 0x00, 0x00        // Padding
		};

		uint32_t ssrc = 0;
		uint8_t seq = 8;
		uint8_t payloadType = 1;
		uint16_t length = 1;
		uint8_t valueMask = 1;

		VbcmItem* item = VbcmItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSsrc() == ssrc);
		REQUIRE(item->GetSequenceNumber() == seq);
		REQUIRE(item->GetPayloadType() == payloadType);
		REQUIRE(item->GetLength() == length);
		REQUIRE((item->GetValue()[item->GetLength() -1] & 1) == valueMask);

		delete item;
	}

	SECTION("parse PsLeiItem")
	{
		uint8_t buffer[] =
		{
			0x00, 0x00, 0x00, 0x01 // SSRC
		};

		uint32_t ssrc = 1;

		PsLeiItem* item = PsLeiItem::Parse(buffer, sizeof(buffer));

		REQUIRE(item);
		REQUIRE(item->GetSsrc() == ssrc);

		delete item;
	}

	SECTION("parse FeedbackPsAfbPacket")
	{
		uint8_t buffer[] =
		{
			0x8F, 0xce, 0x00, 0x03, // RTCP common header
			0x00, 0x00, 0x00, 0x00, // Sender SSRC
			0x00, 0x00, 0x00, 0x00, // Media SSRC
			0x00, 0x00, 0x00, 0x01  // Data
		};

		size_t dataSize = 4;
		uint8_t dataBitmask = 1;

		FeedbackPsAfbPacket* packet = FeedbackPsAfbPacket::Parse(buffer, sizeof(buffer));

		REQUIRE(packet);
		REQUIRE((packet->GetData()[dataSize -1] & 1) == dataBitmask);

		delete packet;
	}
}