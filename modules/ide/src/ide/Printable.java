/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ide;

/**
 *
 * @author mipt
 */
public interface Printable
{
  void print(String message); 
  void println(String message); 
  void log(String prefix, String message);
  void logln(String prefix, String message);
}
