import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;
import inet.applications.udpapp.UdpBasicBurst;

channel Link extends DatarateChannel
{
    datarate = 100Mbps;
    delay = 1ms;
}

network CombinedNetwork
{
    @display("bgb=750,500");
    types:
        simple RouterModule
        {
            parameters:
                @display("i=block/router");
            gates:
                inout ethg[8];
                output outToMonitor;
                input inFromAnalyzer;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=678.5,60.8;i=block/cogwheel");
        }
        server: ServerModule {
            @display("p=363.8,375.4;i=device/server");
        }
        attacker1: CustomBurstSender {
            @display("p=363.1,60.8");
        }
        attacker2: CustomBurstSender {
            @display("p=297.7,65.4");
        }
        attacker3: CustomBurstSender {
            @display("p=244.6,90.8");
        }
        attacker4: CustomBurstSender {
            @display("p=193.1,110.8");
        }
        attacker5: CustomBurstSender {
            @display("p=142.3,140.8");
        }
        attacker6: CustomBurstSender {
            @display("p=90,185.4");
        }
        monitor: CSNMPMonitor {
            @display("p=525.4,240.8");
        }
        collector: CFlowCollector {
            @display("p=678.5,240.8;i=block/bucket");
        }
        analyzer: CFlowAnalyzer {
            @display("p=678.5,375.4;i=block/classifier");
        }
        coreRouter: RouterModule {
            @display("p=364.6,240.8;i=device/hub");
            gates:
                ethg[9];
        }
        goodClient: CustomBurstSender {
            @display("p=60,240.8;i=device/pc");
        }

    connections allowunconnected:

        attacker1.ethg <--> Link <--> coreRouter.ethg[0];
        attacker2.ethg <--> Link <--> coreRouter.ethg[1];
        attacker3.ethg <--> Link <--> coreRouter.ethg[2];
        attacker4.ethg <--> Link <--> coreRouter.ethg[3];
        attacker5.ethg <--> Link <--> coreRouter.ethg[4];
        attacker6.ethg <--> Link <--> coreRouter.ethg[5];
        goodClient.ethg <--> Link <--> coreRouter.ethg[6];

        coreRouter.ethg[7] <--> Link <--> server.ethg;
        coreRouter.outToMonitor --> monitor.in;

        monitor.outToFlowCollector --> collector.packetIn;
        monitor.triggerStage2 --> collector.controlIn;
        collector.packetOut --> analyzer.in;
        analyzer.outToRouter --> coreRouter.inFromAnalyzer;
}
