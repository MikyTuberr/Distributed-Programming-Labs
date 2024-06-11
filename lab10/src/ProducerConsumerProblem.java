import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

public class ProducerConsumerProblem {
    private static int xCount;
    private static int yCount;
    private static int zCount;
    private static int bufferACapacity;
    private static int bufferBCapacity;
    private static int minProductionTime;
    private static int maxProductionTime;
    private static int minConsumptionTime;
    private static int maxConsumptionTime;
    private static float yPriorityConsumption;

    public static void main(String[] args) throws IOException {
        loadConfig();

        Warehouse warehouseA = new Warehouse(bufferACapacity);
        Warehouse warehouseB = new Warehouse(bufferBCapacity);

        List<Thread> threads = new ArrayList<>();

        for (int i = 0; i < xCount; i++) {
            threads.add(new Thread(new Producer(warehouseA, minProductionTime, maxProductionTime), "Producer-X-" + i));
        }

        for (int i = 0; i < yCount; i++) {
            threads.add(new Thread(new WorkerY(warehouseA, warehouseB, minConsumptionTime, maxConsumptionTime,
                    minProductionTime, maxProductionTime, yPriorityConsumption), "Worker-Y-" + i));
        }

        for (int i = 0; i < zCount; i++) {
            threads.add(new Thread(new Consumer(warehouseB, minConsumptionTime, maxConsumptionTime), "Consumer-Z-" + i));
        }

        threads.forEach(Thread::start);

        new Thread(() -> {
            try (FileWriter fileWriter = new FileWriter("buffer_status.csv")) {
                fileWriter.write("Time,Resource A,Resource B\n");
                while (warehouseA.isRunning() && warehouseB.isRunning()) {
                    Thread.sleep(5000);
                    int resourceACount = warehouseA.getProductsCount();
                    int resourceBCount = warehouseB.getProductsCount();
                    System.out.println("Resource A: " + resourceACount + ", Resource B: " + resourceBCount);
                    fileWriter.append(String.valueOf(System.currentTimeMillis())).append(",").
                            append(String.valueOf(resourceACount)).append(",").append(String.valueOf(resourceBCount)).
                            append("\n");
                }
            } catch (InterruptedException | IOException e) {
                e.printStackTrace();
            }
        }).start();

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            warehouseA.stop();
            warehouseB.stop();
            threads.forEach(thread -> {
                try {
                    thread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            });
            System.out.println("Final Resource A: " + warehouseA.getProductCount(Products.PIZZA));
            System.out.println("Final Resource B: " + warehouseB.getProductCount(Products.PASTA));
        }));
    }

    private static void loadConfig() {
        Properties properties = new Properties();
        try {
            properties.load(ProducerConsumerProblem.class.getResourceAsStream("/config.properties"));
            xCount = Integer.parseInt(properties.getProperty("xCount"));
            yCount = Integer.parseInt(properties.getProperty("yCount"));
            zCount = Integer.parseInt(properties.getProperty("zCount"));
            bufferACapacity = Integer.parseInt(properties.getProperty("bufferACapacity"));
            bufferBCapacity = Integer.parseInt(properties.getProperty("bufferBCapacity"));
            minProductionTime = Integer.parseInt(properties.getProperty("minProductionTime"));
            maxProductionTime = Integer.parseInt(properties.getProperty("maxProductionTime"));
            minConsumptionTime = Integer.parseInt(properties.getProperty("minConsumptionTime"));
            maxConsumptionTime = Integer.parseInt(properties.getProperty("maxConsumptionTime"));
            yPriorityConsumption = Float.parseFloat(properties.getProperty("yPriorityConsumption"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
