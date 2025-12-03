simple CustomBurstSender {
    parameters:
        @display("i=device/pc");
        double startTime @unit(s);
        double burstDuration @unit(s);
        double sleepDuration @unit(s);
        double sendInterval @unit(s);
    gates:
        inout ethg;
}
