/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ide;

import java.util.LinkedList;
import java.util.Queue;

/**
 *
 * @author mipt
 */
public class SyncQueue
{

  Queue<String> queue;

  public SyncQueue()
  {
    queue = new LinkedList<>();
  }

  public synchronized void clear()
  {
    queue.clear();
  }

  public synchronized void add(String str)
  {
    queue.add(str);
  }

  public synchronized String poll()
  {
    return queue.poll();
  }

}
