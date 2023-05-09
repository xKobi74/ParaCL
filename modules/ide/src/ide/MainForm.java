/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ide;

import com.sun.org.apache.xerces.internal.impl.xs.opti.DefaultDocument;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.KeyEvent;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import static java.util.stream.Collectors.toMap;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.UndoableEditEvent;
import javax.swing.event.UndoableEditListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.undo.UndoManager;
import org.ini4j.Ini;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
/**
 *
 * @author mipt
 */
public class MainForm extends javax.swing.JFrame
{

  public static String paraclPath;
  public static String configPath;
  public static SyncQueue inputQueue;
  Map<String, Map<String, String>> config;
  public static Color backgroundColor;
  public static int curFontSize;
  public static File curFile = null;
  public static int curWindowSizeX;
  public static int curWindowSizeY;
  public static boolean opt_dump = false;
  public static boolean opt_time = true;
  public MyUndoableEditListener undoManager; 

  //output message to "IDE terminal" in format <prefix>+<message>
  private void log(String prefix, String message)
  {
    if (message == null)
    {
      return;
    }
    textAreaOutput.setText(textAreaOutput.getText() + prefix + message);
  }

  //output message to "IDE terminal" in format <prefix>+<message>+'\n'
  private void logln(String prefix, String message)
  {
    if (message == null)
    {
      return;
    }
    log(prefix, message + '\n');
  }

  

public void append(String s, javax.swing.JTextPane pane){
   try {
      Document doc = pane.getDocument();
      doc.insertString(doc.getLength(), s, null);
   } catch(BadLocationException exc) {
      exc.printStackTrace();
   }
}
//output text to "IDE editor" in format <message>
  private void output(String text)
  {
    if (text == null)
    {
      return;
    }
    append(text, textAreaInput);
  }

  //output text to "IDE editor" in format <message>+'\n'
  private void outputln(String text)
  {
    output(text + '\n');
  }

  //clear "IDE editor"
  private void inputClear()
  {
    textAreaInput.setText("");
  }

  //return text from editor
  private String input()
  {
    return textAreaInput.getText();
  }

  //read File and output it to text editor
  private boolean downloadFile(File iFile)
  {
    if (iFile == null)
    {
      return false;
    }
    if (!iFile.exists())
    {
      System.out.println("File " + iFile + " does not exist");
      logln("Error: ", "File " + iFile + " does not exist");
      return false;
    }
    if (!iFile.canRead())
    {
      System.out.println("File " + iFile + " can not be read");
      logln("Error: ", "File " + iFile + " can not be read");
      return false;
    }
    try (FileReader reader = new FileReader(iFile))
    {
      char[] buf = new char[256];
      int c;
      while ((c = reader.read(buf)) > 0)
      {
        if (c <= 256)
        {
          buf = Arrays.copyOf(buf, c);
          output(new String(buf));
        }
      }
    } catch (Exception ex)
    {
      System.out.println(ex);
      logln("Error: ", "Promblems with file " + iFile + " reading");
      return false;
    }
    System.out.println("menu->file->open->" + iFile);
    logln("Message: ", "File " + iFile + " sucessfully download");
    undoManager.discardAllEdits();
    return true;
  }

  //upload text ftom text editor to File
  private boolean uploadFile(File oFile)
  {
    if (oFile == null)
    {
      System.out.println("File does not exist");
      logln("Error: ", "File does not exist");
      return false;
    }
    if (!oFile.exists())
    {
      System.out.println("File " + oFile + " does not exist");
      logln("Error: ", "File " + oFile + " does not exist");
      return false;
    }
    if (!oFile.canWrite())
    {
      System.out.println("File " + oFile + " can not be written");
      logln("Error: ", "File " + oFile + " can not be written");
      return false;
    }
    try (FileWriter writer = new FileWriter(oFile))
    {
      writer.write(input());
    } catch (Exception ex)
    {
      System.out.println(ex);
      logln("Error: ", "Promblems with file " + oFile + " writing");
      return false;
    }
    System.out.println("menu->file->save->" + oFile);
    logln("Message: ", "File " + oFile + " sucessfully saved");
    return true;
  }

  //create file by its oFile
  private boolean createFile(File oFile)
  {
    if (oFile == null)
    {
      return false;
    }
    try
    {
      boolean created = oFile.createNewFile();
      if (created)
      {
        System.out.println("menu->file->saveas->" + oFile);
        logln("Message: ", "File " + oFile + " sucessfully created");
        return true;
      }
    } catch (IOException ex)
    {
      System.out.println(ex);
    }
    logln("Error: ", "Promblems with file " + oFile + " creating");
    return false;
  }

  //print buffer to terminal
  private void readBuf(BufferedReader reader) throws IOException
  {
    String line;
    while (reader.ready())
    {
      if ((line = reader.readLine()) != null)
      {
        logln("> ", line);
        System.out.println(line);
      }
    }
  }

  private void checkOpts() 
  {
    opt_dump = jCheckBoxMenuDump.getState();
    opt_time = jCheckBoxMenuTime.getState();
  }
  
  private void addOpts(List<String> args) {
      checkOpts();
      if (opt_time) args.add("--time-stamp");
      if (opt_dump) args.add("--dump-tree");
  }
  
  private void drawGraph(String fname) throws Exception {
      List<String> command = new ArrayList<String>();
      command.add("dot");
      command.add("-Tpng");
      command.add(fname);
      command.add("-o");
      command.add(fname+".png");
      ProcessBuilder processBuilder = new ProcessBuilder(command);
      processBuilder.start();
  }

    //compile String code with ParaCL
  private void compileCode(String code) throws Exception
  {
    if (curFile == null)
    {
      logln("Error: ", "Save code to file for execution");
      return;
    }
    setRunEnabled(false);
    uploadFile(curFile);
    List<String> command = new ArrayList<>();
    command.add(paraclPath);
    command.add(curFile.toString());
    command.add("--build");
    
    addOpts(command);
    
    ProcessBuilder processBuilder = new ProcessBuilder(command);
    Process process = processBuilder.start();
    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
    BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
    String line;
    while (process.isAlive())
    {
      readBuf(reader);
      readBuf(errorReader);
      Thread.sleep(10);
    }
    readBuf(reader);
    readBuf(errorReader);
    reader.close();
    errorReader.close();
    setRunEnabled(true);
    if (opt_dump) drawGraph("out.dot");
  }

  //compile String code with ParaCL
  private void executeCode() throws Exception
  {
    if (curFile == null)
    {
      logln("Error: ", "Save code to file for execution");
      return;
    }
    setRunEnabled(false);
    uploadFile(curFile);
    List<String> command = new ArrayList<>();
    command.add(paraclPath);
    command.add(curFile.toString());
    
    addOpts(command);
    
    ProcessBuilder processBuilder = new ProcessBuilder(command);
    Process process = processBuilder.start();

    BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
    BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
    inputQueue.clear();
    String line;
    while (process.isAlive())
    {
      readBuf(reader);
      readBuf(errorReader);
      while ((line = inputQueue.poll()) != null)
      {
        writer.write(line);
        writer.newLine();
      }
      writer.flush();

      Thread.sleep(10);
    }
    readBuf(reader);
    readBuf(errorReader);
    reader.close();
    errorReader.close();
    writer.close();
    setRunEnabled(true);
    if (opt_dump) drawGraph("out.dot");
  }

  private Map<String, Map<String, String>> downloadConfig(String path)
  {
    File configFile = new File(path);
    Ini ini;
    try
    {
      ini = new Ini(configFile);
      return ini.entrySet().stream()
        .collect(toMap(Map.Entry::getKey, Map.Entry::getValue));
    } catch (IOException ex)
    {
      logln("Error: ", "Can't get acess to config file");
      System.out.println(ex);
      return null;
    }
  }

  private void uploadConfig(String path, Map<String, Map<String, String>> config)
  {
    File configFile = new File(path);
    Ini ini;
    try
    {
      ini = new Ini();
      for (String section : config.keySet())
      {
        for (String param : config.get(section).keySet())
        {
          ini.put(section, param, config.get(section).get(param));
        }
      }
      ini.store(configFile);
    } catch (IOException ex)
    {
      logln("Error: ", "Can't get acess to config file");
      System.out.println(ex);
    }
  }
  
  private void setColor(Color color)
  {
    
    Color brightColor = color.brighter();
    Color darkColor = color.darker();
    textAreaInput.setBackground(color);
    textFieldInput.setBackground(darkColor);
    textAreaOutput.setBackground(darkColor);
    jMenuBar.setBackground(brightColor);
    jColorChooser.setBackground(color);
    jDialogColor.setBackground(color);
    jFileChooser.setBackground(color);
    jMenuFile.setBackground(brightColor);
    jMenuOptions.setBackground(brightColor);
    jMenuFileClose.setBackground(brightColor);
    jMenuItemFileOpen.setBackground(brightColor);
    jMenuItemFileSave.setBackground(brightColor);
    jMenuItemFileSaveAs.setBackground(brightColor);
    jMenuRun.setBackground(brightColor);
    jMenuRunBuild.setBackground(brightColor);
    jMenuRunExecute.setBackground(brightColor);
    jMenuSettings.setBackground(brightColor);
    jMenuView.setBackground(brightColor);
    jMenuViewBackground.setBackground(brightColor);
    jUndo.setBackground(brightColor);
    jRedo.setBackground(brightColor);
    jMenuViewFont.setBackground(brightColor);
    jCheckBoxMenuLastFile.setBackground(brightColor);
    jCheckBoxMenuDump.setBackground(brightColor);
    jCheckBoxMenuTime.setBackground(brightColor);
    jCheckBoxMenuHighlight.setBackground(brightColor);
    for (Component item : this.rootPane.getComponents())
      item.validate();
    
  }
  private void setFont(int size)
  {
    Font baseFont = textAreaInput.getFont();
    Font font = new Font(baseFont.getFamily(), 0, size);
    
    textAreaInput.setFont(font);
    textFieldInput.setFont(font);
    textAreaOutput.setFont(font);
    jMenuBar.setFont(font);
    jColorChooser.setFont(font);
    jDialogColor.setFont(font);
    jFileChooser.setFont(font);
    jMenuFile.setFont(font);
    jMenuFileClose.setFont(font);
    jMenuItemFileOpen.setFont(font);
    jMenuItemFileSave.setFont(font);
    jMenuItemFileSaveAs.setFont(font);
    jMenuRun.setFont(font);
    jMenuRunBuild.setFont(font);
    jMenuRunExecute.setFont(font);
    jMenuSettings.setFont(font);
    jMenuView.setFont(font);
    jMenuViewBackground.setFont(font);
    jMenuViewFont.setFont(font);
    jCheckBoxMenuLastFile.setFont(font);
    jMenuOptions.setFont(font);
    jCheckBoxMenuDump.setFont(font);
    jCheckBoxMenuTime.setFont(font);
    jCheckBoxMenuHighlight.setFont(font);
    jUndo.setFont(font);
    jRedo.setFont(font);
    for (Component item : this.rootPane.getComponents())
      item.validate();
  }
    
  private void restoreConfig(Map<String, Map<String, String>> config)
  {
     Map<String, String> settings = config.get("settings");
     boolean startWithLastFile = "true".equals(settings.get("start-with-last-file"));
     jCheckBoxMenuLastFile.setState(startWithLastFile);
     String lastFile = settings.get("last-file");
     if (startWithLastFile)
     {
      if (lastFile.equals(""))
        curFile = null;
      else
        curFile = new File(lastFile);
      downloadFile(curFile);
     }
     String windowSizeX = settings.get("window-size-x");
     String windowSizeY = settings.get("window-size-y");
     curWindowSizeX = Integer.parseInt(windowSizeX);
     curWindowSizeY = Integer.parseInt(windowSizeY);
     this.setSize(curWindowSizeX, curWindowSizeY);
     
     Map<String, String> view = config.get("view");
     String color = view.get("background-color");
     backgroundColor = new Color(Integer.parseInt(color));
     setColor(backgroundColor);
     String fontSize = view.get("font-size");
     curFontSize = Integer.parseInt(fontSize);
     setFont(curFontSize);
  }
  
  private Map<String, Map<String, String>> storeConfig(Map<String, Map<String, String>> config)
  {
     Map<String, String> settings = config.get("settings");
     boolean startWithLastFile = jCheckBoxMenuLastFile.getState();
     settings.replace("start-with-last-file", "false"); 
     if (startWithLastFile)  
       settings.replace("start-with-last-file", "true");
     String lastFile;
     if (curFile == null)
       lastFile = "";
     else 
       lastFile = curFile.toString();
     settings.replace("last-file", lastFile);
     settings.replace("window-size-x", Integer.toString(curWindowSizeX));
     settings.replace("window-size-y", Integer.toString(curWindowSizeY));
     config.replace("settings", settings);
     
     Map<String, String> view = config.get("view");
     view.replace("background-color", Integer.toString(backgroundColor.getRGB()));
     view.replace("font-size", Integer.toString(curFontSize));
     config.replace("view", view);
     
     return config;
  }

  /**
   * Creates new form MainForm
   */
  public MainForm()
  {
    String workingDirectory = Paths.get(".").toAbsolutePath().toString();
    workingDirectory = workingDirectory.substring(0, workingDirectory.length() - 1);
    paraclPath = workingDirectory + "../../build/modules/bison/pcli";
    configPath = workingDirectory + "src/config.ini";
    inputQueue = new SyncQueue();
    initComponents();
    initUndoManager();
    //HACK: color picker turned off to prvent changes
    //jMenuViewBackground.setEnabled(false);
    config = downloadConfig(configPath);
    restoreConfig(config);
  }
  

  private void initUndoManager() {
    undoManager = new MyUndoableEditListener();
    undoManager. addChangeListener( new ChangeListener() {
         @Override
         public void stateChanged(ChangeEvent e) {
        jUndo.setEnabled(undoManager.canUndo());
        jRedo.setEnabled(undoManager.canRedo());
    }
    }
    );
    textAreaInput.getDocument().addUndoableEditListener(undoManager);
  }
  
 /* private void updateUndoManager() {
      //jUndo.getParent().validate();
      //jRedo.getParent().validate();
      System.out.println(undoManager.canUndo());
      jUndo.setEnabled(undoManager.canUndo());
      jRedo.setEnabled(undoManager.canRedo());
  
  }*/
  /**
   * This method is called from within the constructor to initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is always
   * regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jFileChooser = new javax.swing.JFileChooser();
        jDialogColor = new javax.swing.JDialog();
        jColorChooser = new javax.swing.JColorChooser();
        jDialogFont = new javax.swing.JDialog();
        jPanelFont = new javax.swing.JPanel();
        jTextPaneFontSize = new javax.swing.JTextPane();
        jSplitPane = new javax.swing.JSplitPane();
        jSplitPaneTerminal = new javax.swing.JSplitPane();
        textFieldInput = new java.awt.TextField();
        textAreaOutput = new java.awt.TextArea();
        jScrollPane1 = new javax.swing.JScrollPane();
        textAreaInput = new ide.syntaxPane.JSyntaxTextPane();
        jMenuBar = new javax.swing.JMenuBar();
        jMenuFile = new javax.swing.JMenu();
        jMenuItemFileOpen = new javax.swing.JMenuItem();
        jMenuItemFileSave = new javax.swing.JMenuItem();
        jMenuItemFileSaveAs = new javax.swing.JMenuItem();
        jMenuFileClose = new javax.swing.JMenuItem();
        jMenuView = new javax.swing.JMenu();
        jMenuViewFont = new javax.swing.JMenuItem();
        jMenuViewBackground = new javax.swing.JMenuItem();
        jUndo = new javax.swing.JMenuItem();
        jRedo = new javax.swing.JMenuItem();
        jMenuSettings = new javax.swing.JMenu();
        jCheckBoxMenuLastFile = new javax.swing.JCheckBoxMenuItem();
        jCheckBoxMenuHighlight = new javax.swing.JCheckBoxMenuItem();
        jMenuOptions = new javax.swing.JMenu();
        jCheckBoxMenuDump = new javax.swing.JCheckBoxMenuItem();
        jCheckBoxMenuTime = new javax.swing.JCheckBoxMenuItem();
        jMenuRun = new javax.swing.JMenu();
        jMenuRunBuild = new javax.swing.JMenuItem();
        jMenuRunExecute = new javax.swing.JMenuItem();

        jFileChooser.setToolTipText("");
        jFileChooser.setName(""); // NOI18N

        jDialogColor.addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowOpened(java.awt.event.WindowEvent evt) {
                jDialogColorWindowOpened(evt);
            }
            public void windowClosing(java.awt.event.WindowEvent evt) {
                jDialogColorWindowClosing(evt);
            }
        });

        jColorChooser.setBorder(javax.swing.BorderFactory.createTitledBorder("Close dialog to apply color"));
        jDialogColor.getContentPane().add(jColorChooser, java.awt.BorderLayout.CENTER);

        jDialogFont.addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                jDialogFontWindowClosing(evt);
            }
        });

        jPanelFont.setBorder(javax.swing.BorderFactory.createTitledBorder("Close dialog to apply font"));
        jPanelFont.setPreferredSize(new java.awt.Dimension(200, 59));
        jPanelFont.setLayout(new java.awt.BorderLayout());

        jTextPaneFontSize.setBorder(javax.swing.BorderFactory.createTitledBorder("font size"));
        jTextPaneFontSize.setPreferredSize(new java.awt.Dimension(200, 37));
        jPanelFont.add(jTextPaneFontSize, java.awt.BorderLayout.PAGE_START);

        jDialogFont.getContentPane().add(jPanelFont, java.awt.BorderLayout.CENTER);

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setMinimumSize(new java.awt.Dimension(204, 204));
        addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                formComponentResized(evt);
            }
        });
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });

        jSplitPane.setBorder(javax.swing.BorderFactory.createTitledBorder("Editor"));
        jSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane.setResizeWeight(0.8);
        jSplitPane.setToolTipText("");

        jSplitPaneTerminal.setBorder(javax.swing.BorderFactory.createTitledBorder("Terminal: top - output, bottom - input"));
        jSplitPaneTerminal.setDividerSize(8);
        jSplitPaneTerminal.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPaneTerminal.setResizeWeight(1.0);
        jSplitPaneTerminal.setToolTipText("");
        jSplitPaneTerminal.setMinimumSize(new java.awt.Dimension(102, 100));
        jSplitPaneTerminal.setName(""); // NOI18N
        jSplitPaneTerminal.setPreferredSize(new java.awt.Dimension(102, 102));
        jSplitPaneTerminal.setRequestFocusEnabled(false);

        textFieldInput.setFont(new java.awt.Font("Ubuntu Mono", 0, 14)); // NOI18N
        textFieldInput.setName(""); // NOI18N
        textFieldInput.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                textFieldInputKeyPressed(evt);
            }
        });
        jSplitPaneTerminal.setBottomComponent(textFieldInput);

        textAreaOutput.setEditable(false);
        textAreaOutput.setFont(new java.awt.Font("Ubuntu Mono", 0, 14)); // NOI18N
        textAreaOutput.addTextListener(new java.awt.event.TextListener() {
            public void textValueChanged(java.awt.event.TextEvent evt) {
                textAreaOutputTextValueChanged(evt);
            }
        });
        jSplitPaneTerminal.setTopComponent(textAreaOutput);

        jSplitPane.setBottomComponent(jSplitPaneTerminal);

        jScrollPane1.setBackground(new java.awt.Color(61, 60, 63));
        jScrollPane1.setViewportView(textAreaInput);

        jSplitPane.setLeftComponent(jScrollPane1);

        getContentPane().add(jSplitPane, java.awt.BorderLayout.CENTER);

        jMenuFile.setText("File");
        jMenuFile.addMenuListener(new javax.swing.event.MenuListener() {
            public void menuCanceled(javax.swing.event.MenuEvent evt) {
            }
            public void menuDeselected(javax.swing.event.MenuEvent evt) {
            }
            public void menuSelected(javax.swing.event.MenuEvent evt) {
                jMenuSelected(evt);
            }
        });

        jMenuItemFileOpen.setText("Open");
        jMenuItemFileOpen.setToolTipText("");
        jMenuItemFileOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFileOpenActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemFileOpen);

        jMenuItemFileSave.setText("Save");
        jMenuItemFileSave.setToolTipText("");
        jMenuItemFileSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFileSaveActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemFileSave);

        jMenuItemFileSaveAs.setText("Save as");
        jMenuItemFileSaveAs.setToolTipText("");
        jMenuItemFileSaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemFileSaveAsActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemFileSaveAs);

        jMenuFileClose.setText("Close");
        jMenuFileClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuFileCloseActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuFileClose);

        jMenuBar.add(jMenuFile);

        jMenuView.setText("View");
        jMenuView.addMenuListener(new javax.swing.event.MenuListener() {
            public void menuCanceled(javax.swing.event.MenuEvent evt) {
            }
            public void menuDeselected(javax.swing.event.MenuEvent evt) {
            }
            public void menuSelected(javax.swing.event.MenuEvent evt) {
                jMenuSelected(evt);
            }
        });

        jMenuViewFont.setText("Font");
        jMenuViewFont.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuViewFontActionPerformed(evt);
            }
        });
        jMenuView.add(jMenuViewFont);

        jMenuViewBackground.setText("Background");
        jMenuViewBackground.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuViewBackgroundActionPerformed(evt);
            }
        });
        jMenuView.add(jMenuViewBackground);

        jUndo.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Z, java.awt.event.InputEvent.CTRL_DOWN_MASK));
        jUndo.setText("Undo");
        jUndo.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jUndoActionPerformed(evt);
            }
        });
        jMenuView.add(jUndo);

        jRedo.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Z, java.awt.event.InputEvent.SHIFT_DOWN_MASK | java.awt.event.InputEvent.CTRL_DOWN_MASK));
        jRedo.setText("Redo");
        jRedo.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jRedoActionPerformed(evt);
            }
        });
        jMenuView.add(jRedo);

        jMenuBar.add(jMenuView);

        jMenuSettings.setText("Settings");
        jMenuSettings.addMenuListener(new javax.swing.event.MenuListener() {
            public void menuCanceled(javax.swing.event.MenuEvent evt) {
            }
            public void menuDeselected(javax.swing.event.MenuEvent evt) {
            }
            public void menuSelected(javax.swing.event.MenuEvent evt) {
                jMenuSelected(evt);
            }
        });

        jCheckBoxMenuLastFile.setSelected(true);
        jCheckBoxMenuLastFile.setText("Start with last file");
        jMenuSettings.add(jCheckBoxMenuLastFile);

        jCheckBoxMenuHighlight.setSelected(true);
        jCheckBoxMenuHighlight.setText("Code highlight");
        jCheckBoxMenuHighlight.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxMenuHighlightActionPerformed(evt);
            }
        });
        jMenuSettings.add(jCheckBoxMenuHighlight);

        jMenuBar.add(jMenuSettings);

        jMenuOptions.setText("Options");

        jCheckBoxMenuDump.setText("Dump AST");
        jCheckBoxMenuDump.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxMenuDumpActionPerformed(evt);
            }
        });
        jMenuOptions.add(jCheckBoxMenuDump);

        jCheckBoxMenuTime.setSelected(true);
        jCheckBoxMenuTime.setText("Measure time");
        jCheckBoxMenuTime.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxMenuTimeActionPerformed(evt);
            }
        });
        jMenuOptions.add(jCheckBoxMenuTime);

        jMenuBar.add(jMenuOptions);

        jMenuRun.setText("Run");
        jMenuRun.addMenuListener(new javax.swing.event.MenuListener() {
            public void menuCanceled(javax.swing.event.MenuEvent evt) {
            }
            public void menuDeselected(javax.swing.event.MenuEvent evt) {
            }
            public void menuSelected(javax.swing.event.MenuEvent evt) {
                jMenuSelected(evt);
            }
        });

        jMenuRunBuild.setText("Build");
        jMenuRunBuild.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuRunBuildActionPerformed(evt);
            }
        });
        jMenuRun.add(jMenuRunBuild);

        jMenuRunExecute.setText("Execute");
        jMenuRunExecute.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuRunExecuteActionPerformed(evt);
            }
        });
        jMenuRun.add(jMenuRunExecute);

        jMenuBar.add(jMenuRun);

        setJMenuBar(jMenuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

  //action on menu item Open that opens and reads the file
  private void jMenuItemFileOpenActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileOpenActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileOpenActionPerformed
    jFileChooser.setSelectedFile(new File(""));
    jFileChooser.showOpenDialog(this);
    File iFile = jFileChooser.getSelectedFile();
    String reserve = input();
    inputClear();
    if (downloadFile(iFile))
    {
      curFile = iFile;
    } else
    {
      output(reserve);
    }
  }//GEN-LAST:event_jMenuItemFileOpenActionPerformed

  //action on menu item Save that saves text from editor to current file
  private void jMenuItemFileSaveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileSaveActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileSaveActionPerformed
    uploadFile(curFile);
  }//GEN-LAST:event_jMenuItemFileSaveActionPerformed

  //action on menu item SaveAs that saves text from editor to file
  private void jMenuItemFileSaveAsActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItemFileSaveAsActionPerformed
  {//GEN-HEADEREND:event_jMenuItemFileSaveAsActionPerformed
    jFileChooser.setSelectedFile(new File(""));
    jFileChooser.showSaveDialog(this);
    File oFile = jFileChooser.getSelectedFile();
    if (createFile(oFile))
    {
      curFile = oFile;
    } else
    {
      return;
    }
    uploadFile(curFile);
  }//GEN-LAST:event_jMenuItemFileSaveAsActionPerformed

  private void setRunEnabled(boolean enable) {
      jMenuRunBuild.setEnabled(enable);
    jMenuRunExecute.setEnabled(enable);
  }
  //action on menu item Build that saves and builds current file
  private void jMenuRunBuildActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuRunBuildActionPerformed
  {//GEN-HEADEREND:event_jMenuRunBuildActionPerformed
    System.out.println("menu->run->build");
    Thread compiler = new Thread(() ->
    {
      try
      {
        compileCode(input());
      } catch (Exception ex)
      {
        System.out.println(ex);
        logln("Error: ", "Compilation fault");
      }
    });
    compiler.start();
  }//GEN-LAST:event_jMenuRunBuildActionPerformed

  //action on Enter to input field that sends text to program
  private void textFieldInputKeyPressed(java.awt.event.KeyEvent evt)//GEN-FIRST:event_textFieldInputKeyPressed
  {//GEN-HEADEREND:event_textFieldInputKeyPressed
    if (evt.getKeyChar() != '\n')
    {
      switch (evt.getKeyCode())
      {
        case KeyEvent.VK_LEFT:
          if (textFieldInput.getCaretPosition() == 0)
          {
            break;
          }
          textFieldInput.setCaretPosition(textFieldInput.getCaretPosition() - 1);
          break;
        case KeyEvent.VK_RIGHT:
          textFieldInput.setCaretPosition(textFieldInput.getCaretPosition() + 1);
          break;
      }
      return;
    }
    System.out.println("Input->Enter");
    logln("< ", textFieldInput.getText());
    inputQueue.add(textFieldInput.getText());
    textFieldInput.setText(" ");

  }//GEN-LAST:event_textFieldInputKeyPressed

  //action on menu item Execute that saves and execute current file
  private void jMenuRunExecuteActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuRunExecuteActionPerformed
  {//GEN-HEADEREND:event_jMenuRunExecuteActionPerformed
    System.out.println("menu->run->execute");
    Thread executer = new Thread(() ->
    {
      try
      {
        executeCode();
      } catch (Exception ex)
      {
        System.out.println(ex);
        logln("Error: ", "Execution fault");
      }
    });

    executer.start();

  }//GEN-LAST:event_jMenuRunExecuteActionPerformed

  //action on appearance of the new message in terminal that scrolls down text
  private void textAreaOutputTextValueChanged(java.awt.event.TextEvent evt)//GEN-FIRST:event_textAreaOutputTextValueChanged
  {//GEN-HEADEREND:event_textAreaOutputTextValueChanged
    textAreaOutput.setCaretPosition(textAreaOutput.getText().length());
  }//GEN-LAST:event_textAreaOutputTextValueChanged

  static int findLeft(String text, int start, char symbol)
  {
    int pos = start;
    while (pos >= 0 && text.charAt(pos) != symbol)
    {
      pos--;
    }
    return pos;
  }

  static int findRight(String text, int start, char symbol)
  {
    int pos = start;
    int end = text.length();
    while (pos < end && text.charAt(pos) != symbol)
    {
      pos++;
    }
    return pos;
  }

  private void jMenuSelected(javax.swing.event.MenuEvent evt)//GEN-FIRST:event_jMenuSelected
  {//GEN-HEADEREND:event_jMenuSelected
    for (Component menu : jMenuBar.getComponents())
    {
      menu.setVisible(false);
      menu.setVisible(true);
    }
  }//GEN-LAST:event_jMenuSelected

  private void formWindowClosing(java.awt.event.WindowEvent evt)//GEN-FIRST:event_formWindowClosing
  {//GEN-HEADEREND:event_formWindowClosing
    config = storeConfig(config);
    uploadConfig(configPath, config);
  }//GEN-LAST:event_formWindowClosing

  private void jMenuViewBackgroundActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuViewBackgroundActionPerformed
  {//GEN-HEADEREND:event_jMenuViewBackgroundActionPerformed
    jDialogColor.setVisible(true);
    jDialogColor.pack();
  }//GEN-LAST:event_jMenuViewBackgroundActionPerformed

  private void jDialogColorWindowOpened(java.awt.event.WindowEvent evt)//GEN-FIRST:event_jDialogColorWindowOpened
  {//GEN-HEADEREND:event_jDialogColorWindowOpened
    jColorChooser.setColor(backgroundColor);
  }//GEN-LAST:event_jDialogColorWindowOpened

  private void jDialogColorWindowClosing(java.awt.event.WindowEvent evt)//GEN-FIRST:event_jDialogColorWindowClosing
  {//GEN-HEADEREND:event_jDialogColorWindowClosing
    backgroundColor = jColorChooser.getColor();
    setColor(backgroundColor);
  }//GEN-LAST:event_jDialogColorWindowClosing

  private void jMenuFileCloseActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuFileCloseActionPerformed
  {//GEN-HEADEREND:event_jMenuFileCloseActionPerformed
    if (!(curFile == null))
      uploadFile(curFile);
    curFile = null;
    inputClear();
  }//GEN-LAST:event_jMenuFileCloseActionPerformed

  private void jMenuViewFontActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuViewFontActionPerformed
  {//GEN-HEADEREND:event_jMenuViewFontActionPerformed
    jDialogFont.setVisible(true);
    jDialogFont.pack();
  }//GEN-LAST:event_jMenuViewFontActionPerformed

  private void jDialogFontWindowClosing(java.awt.event.WindowEvent evt)//GEN-FIRST:event_jDialogFontWindowClosing
  {//GEN-HEADEREND:event_jDialogFontWindowClosing
    curFontSize = Integer.parseInt(jTextPaneFontSize.getText().trim());
    setFont(curFontSize);
  }//GEN-LAST:event_jDialogFontWindowClosing

  private void formComponentResized(java.awt.event.ComponentEvent evt)//GEN-FIRST:event_formComponentResized
  {//GEN-HEADEREND:event_formComponentResized
     Dimension dim = this.getSize();
     curWindowSizeX = dim.width;
     curWindowSizeY = dim.height;
  }//GEN-LAST:event_formComponentResized

    private void jCheckBoxMenuDumpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxMenuDumpActionPerformed
        opt_dump = jCheckBoxMenuDump.getState();
    }//GEN-LAST:event_jCheckBoxMenuDumpActionPerformed

    private void jCheckBoxMenuTimeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxMenuTimeActionPerformed
        opt_time = jCheckBoxMenuTime.getState();
    }//GEN-LAST:event_jCheckBoxMenuTimeActionPerformed

    private void jUndoActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jUndoActionPerformed
        System.out.println("Undo called");
        
        undoManager.undo();
    }//GEN-LAST:event_jUndoActionPerformed

    private void jRedoActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jRedoActionPerformed
        undoManager.redo();
    }//GEN-LAST:event_jRedoActionPerformed

    private void jCheckBoxMenuHighlightActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxMenuHighlightActionPerformed
        textAreaInput.setStyled(jCheckBoxMenuHighlight.getState());
        uploadFile(curFile);
        inputClear();
        downloadFile(curFile);    
    }//GEN-LAST:event_jCheckBoxMenuHighlightActionPerformed

  /**
   * @param args the command line arguments
   */
  public static void main(String args[])
  {
    /* Set the Nimbus look and feel */
    //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
    /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
     */
    try
    {
      for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels())
      {
        if ("Nimbus".equals(info.getName()))
        {
          javax.swing.UIManager.setLookAndFeel(info.getClassName());
          break;
        }
      }
    } catch (ClassNotFoundException ex)
    {
      java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (InstantiationException ex)
    {
      java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (IllegalAccessException ex)
    {
      java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (javax.swing.UnsupportedLookAndFeelException ex)
    {
      java.util.logging.Logger.getLogger(MainForm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    }
    //</editor-fold>

  }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuDump;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuHighlight;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuLastFile;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuTime;
    private javax.swing.JColorChooser jColorChooser;
    private javax.swing.JDialog jDialogColor;
    private javax.swing.JDialog jDialogFont;
    private javax.swing.JFileChooser jFileChooser;
    private javax.swing.JMenuBar jMenuBar;
    private javax.swing.JMenu jMenuFile;
    private javax.swing.JMenuItem jMenuFileClose;
    private javax.swing.JMenuItem jMenuItemFileOpen;
    private javax.swing.JMenuItem jMenuItemFileSave;
    private javax.swing.JMenuItem jMenuItemFileSaveAs;
    private javax.swing.JMenu jMenuOptions;
    private javax.swing.JMenu jMenuRun;
    private javax.swing.JMenuItem jMenuRunBuild;
    private javax.swing.JMenuItem jMenuRunExecute;
    private javax.swing.JMenu jMenuSettings;
    private javax.swing.JMenu jMenuView;
    private javax.swing.JMenuItem jMenuViewBackground;
    private javax.swing.JMenuItem jMenuViewFont;
    private javax.swing.JPanel jPanelFont;
    private javax.swing.JMenuItem jRedo;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JSplitPane jSplitPane;
    private javax.swing.JSplitPane jSplitPaneTerminal;
    private javax.swing.JTextPane jTextPaneFontSize;
    private javax.swing.JMenuItem jUndo;
    private ide.syntaxPane.JSyntaxTextPane textAreaInput;
    private java.awt.TextArea textAreaOutput;
    private java.awt.TextField textFieldInput;
    // End of variables declaration//GEN-END:variables

}
