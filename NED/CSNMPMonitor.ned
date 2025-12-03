simple CSNMPMonitor
{
    parameters:
        @display("i=block/cogwheel");
        int    historySize     = default(10);
        int    packetCount     = default(0);
        double alpha           = default(0.2);
        double threshold       = default(2.0);
        double initialRate     = default(100);
        double samplePeriod @unit(s) = default(1s);
        double trainingPeriod @unit(s) = default(900s);
        double absoluteVolumeThreshold = default(0);
    	double floorFactor @default(1.05);
    	
    @signal[snmpTrigger](type=simtime_t);
    @statistic[snmpTrigger](
        title="SNMP trigger times"; 
        source=snmpTrigger; 
        record=vector
    );
    
    @signal[sampleRate](type=int);
    @statistic[sampleRate](
        title="Per-second packet samples";
        source=sampleRate;
        record=vector
    );
    	
    gates:
        input  in;
        output outToFlowCollector;
        output triggerStage2;

}
