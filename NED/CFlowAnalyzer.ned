simple CFlowAnalyzer
{
    parameters:
        int portPacketThreshold = default(10000); 
        int topN = default(5);
        int goodClientPort = -1;
        @signal[attackFlow](type=string);
        @statistic[attackFlow](
            title="Identified Attack Flows";
            source=attackFlow;
            record=vector;
        );
    gates:
        input in;
        output outToRouter;
}
