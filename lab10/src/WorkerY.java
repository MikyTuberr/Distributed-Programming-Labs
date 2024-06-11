import java.util.concurrent.ThreadLocalRandom;

public class WorkerY implements Runnable {
    private final Warehouse warehouseA;
    private final Warehouse warehouseB;
    private final int minConsumptionTime;
    private final int maxConsumptionTime;
    private final int minProductionTime;
    private final int maxProductionTime;
    private final float priorityConsumption;

    public WorkerY(Warehouse warehouseA, Warehouse warehouseB, int minConsumptionTime, int maxConsumptionTime,
                   int minProductionTime, int maxProductionTime, float priorityConsumption) {
        this.warehouseA = warehouseA;
        this.warehouseB = warehouseB;
        this.minConsumptionTime = minConsumptionTime;
        this.maxConsumptionTime = maxConsumptionTime;
        this.minProductionTime = minProductionTime;
        this.maxProductionTime = maxProductionTime;
        this.priorityConsumption = priorityConsumption;
    }

    @Override
    public void run() {
        while (warehouseA.isRunning() && warehouseB.isRunning()) {
            try {
                if (priorityConsumption == 1) {
                    consumeFromA();
                }
                else if(priorityConsumption == -1) {
                    produceToB();
                }
                if (Math.random()* 2 - 1 < priorityConsumption) {
                    consumeFromA();
                } else {
                    produceToB();
                }
            } catch (InterruptedException e) {
                System.out.println(Thread.currentThread().getName() + " shutting down...");
                return;
            }
        }
        System.out.println(Thread.currentThread().getName() + " shutting down...");
    }

    private void consumeFromA() throws InterruptedException {
        Products product = Products.values()[ThreadLocalRandom.current().nextInt(Products.values().length)];
        //int quantity = ThreadLocalRandom.current().nextInt(1, 6);
        int takenQuantity = warehouseA.takeProduct(product, 1);
        System.out.println(Thread.currentThread().getName() + " consumed " + takenQuantity + " of " + product + " from Warehouse A");
        Thread.sleep(ThreadLocalRandom.current().nextInt(minConsumptionTime, maxConsumptionTime));
    }

    private void produceToB() throws InterruptedException {
        Products product = Products.values()[ThreadLocalRandom.current().nextInt(Products.values().length)];
        //int quantity = ThreadLocalRandom.current().nextInt(1, 11);
        warehouseB.addProduct(product, 1);
        System.out.println(Thread.currentThread().getName() + " produced " + 1 + " of " + product + " to Warehouse B");
        Thread.sleep(ThreadLocalRandom.current().nextInt(minProductionTime, maxProductionTime));
    }
}
