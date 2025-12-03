simple CFlowCollector
{
    parameters:
        int    maxFlows         = default(10000);
        double collectionWindow @unit(s) = default(5s);
        @signal[flowsExported](type=int);
        @statistic[flowsExported](
            title = "FlowRecords emitted";
            source = flowsExported;
            record = vector;
        );
        @signal[port0_countVec](type=int);
		@statistic[port0_countVec](title="Port0 Count per Window"; source=port0_countVec; record=vector);
		@signal[port1_countVec](type=int);
		@statistic[port1_countVec](title="Port1 Count per Window"; source=port1_countVec; record=vector);
		@signal[port2_countVec](type=int);
		@statistic[port2_countVec](title="Port2 Count per Window"; source=port2_countVec; record=vector);
		@signal[port3_countVec](type=int);
		@statistic[port3_countVec](title="Port3 Count per Window"; source=port3_countVec; record=vector);
		@signal[port4_countVec](type=int);
		@statistic[port4_countVec](title="Port4 Count per Window"; source=port4_countVec; record=vector);
		@signal[port5_countVec](type=int);
		@statistic[port5_countVec](title="Port5 Count per Window"; source=port5_countVec; record=vector);
		@signal[port6_countVec](type=int);
		@statistic[port6_countVec](title="Port6 Count per Window"; source=port6_countVec; record=vector);
    gates:
        input  controlIn;
        input packetIn;
        output packetOut;
}
