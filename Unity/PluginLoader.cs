using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class PluginLoader : MonoBehaviour
{
    [DllImport("QtPlugin")]
    private static extern void showQtApp();

    // Start is called before the first frame update
    void Start()
    {
        showQtApp();
    }

    // Update is called once per frame
    void Update()
    {

    }
}
