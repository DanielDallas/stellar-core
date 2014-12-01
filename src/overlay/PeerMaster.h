#ifndef __PEERMASTER__
#define __PEERMASTER__

#include "Peer.h"
#include "PeerDoor.h"
#include "overlay/ItemFetcher.h"
#include "overlay/Floodgate.h"
#include <vector>
#include <thread>
#include "generated/stellar.hh"
#include "overlay/OverlayGateway.h"
#include "overlay/PreferredPeers.h"

using namespace std;
/*
Maintain the set of peers we are connected to
*/
namespace stellar
{

	class PeerMaster : public OverlayGateway
	{
		std::thread mPeerThread;

        // peers we are connected to
		vector<Peer::pointer> mPeers;
		PeerDoor mDoor;
		QSetFetcher mQSetFetcher;
        ApplicationPtr mApp;
        PreferredPeers mPreferredPeers;

		void addConfigPeers();

		void run();
        void tick();
        boost::asio::deadline_timer* mTimer;
	public:
		boost::asio::io_service* mIOservice;
		Floodgate mFloodGate;

		PeerMaster();
		~PeerMaster();

        void setApplication(ApplicationPtr app) { mApp = app; }
		//////// GATEWAY FUNCTIONS
		void ledgerClosed(LedgerPtr ledger);

		QuorumSet::pointer fetchQuorumSet(stellarxdr::uint256& itemID, bool askNetwork){ return(mQSetFetcher.fetchItem(itemID,askNetwork)); }
        void recvFloodedMsg(stellarxdr::uint256 index, StellarMessagePtr msg, uint32_t ledgerIndex, Peer::pointer peer) { mFloodGate.addRecord(index, msg, ledgerIndex, peer);  }
        void doesntHaveQSet(stellarxdr::uint256 index, Peer::pointer peer) { mQSetFetcher.doesntHave(index, peer,mApp); }

        void broadcastMessage(StellarMessagePtr msg, Peer::pointer peer);
        void recvQuorumSet(QuorumSet::pointer qset);
		//////


		void start();
		void addPeer(Peer::pointer peer);
		void dropPeer(Peer::pointer peer);
        bool isPeerAccepted(Peer::pointer peer);

		Peer::pointer getRandomPeer();
		Peer::pointer getNextPeer(Peer::pointer peer); // returns NULL if the passed peer isn't found

		
		void broadcastMessage(stellarxdr::uint256& msgID);
		void broadcastMessage(StellarMessagePtr msg, vector<Peer::pointer>& skip);
	};
}

#endif