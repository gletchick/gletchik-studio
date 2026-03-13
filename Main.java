package test;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        // Создаем объект Scanner для чтения из стандартного потока ввода (System.in)
        Scanner scanner = new Scanner(System.in);

        System.out.println("Введите ваше имя:");

        // Программа остановится здесь и будет ждать данных из твоего m_stdinPipe
        if (scanner.hasNextLine()) {
            String name = scanner.nextLine();
            System.out.println("Привет, " + name + "! Ваша IDE работает корректно.");
        }

        scanner.close();
    }
}