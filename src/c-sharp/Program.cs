using System.Net;
using System.Net.Sockets;
using System.Resources;
using System.Text;
using System.Text.RegularExpressions;

namespace j2iwol.net
{
    internal class Program
    {

        [Flags]
        enum AppSwitches
        {
            SHOW_HELP = 1, 
            QUIET_MODE = 2,
            SHOW_ABOUT = 4
        }
        static Regex MacAddressRegex { get; set; } = new Regex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", RegexOptions.IgnoreCase);
        static void SendWOL(IEnumerable<byte> MACAddress)
        {
            byte[] packet = new byte[102];
            for (int i = 0; i < 6; i++)
                packet[i] = 0xFF;
            for (int i = 0; i < 102-6; i ++)
            {
                packet[i + 6] = MACAddress.ElementAt(i%6);
            }
            UdpClient client = new UdpClient();
            client.Client.Bind(new IPEndPoint(IPAddress.Any, 0));
            client.Send(packet, packet.Length, new IPEndPoint(IPAddress.Broadcast, 9));

        }

        static void SendWOL(String MACAddress)
        {
            var parts = MACAddress.Split(new char[] { ':', '-' });
            if (parts.Length != 6)
                return;
            byte[] mac = new byte[6];
            for (int i = 0; i < 6; i++)
                mac[i] = Convert.ToByte(parts[i], 16);
            SendWOL(mac);
        }


        static void ShowHelp(ResourceSet resourceSet)
        {
            var helpString = resourceSet.GetString("HelpString");
            Console.Out.WriteLine(helpString);
        }
        static void ShowAbout(ResourceSet resourceManager)
        {
            var aboutString = resourceManager.GetString("AboutString");
            Console.Out.WriteLine(aboutString);
        }

        static int GetSwitches(string[] args)
        {
            int switches = 0;
            foreach (var arg in args)
            {
                if (arg == "-h" || arg == "--help" || arg == "--?" || arg == "/h" || arg == "/?")
                    switches |= (int)AppSwitches.SHOW_HELP;
                if (arg == "-q" || arg == "--quiet")
                    switches |= (int)AppSwitches.QUIET_MODE;
                if (arg == "-a" || arg == "--about")
                    switches |= (int)AppSwitches.SHOW_ABOUT;
            }
            return switches;
        }

        static List<String> GetMacAddressList(String[] args)
        {
            List<String> retVal = new List<String>();
            foreach (var arg in args)
            {
                if (MacAddressRegex.IsMatch(arg))
                    retVal.Add(arg);
            }
            return retVal;
        }
        static void Main(string[] args)
        {
            var resourceManager = Resources.ResourceManager.GetResourceSet(System.Globalization.CultureInfo.CurrentCulture, true, true);
            if (resourceManager == null)
            {
                Console.Error.WriteLine("Resource Manager not found");
                return;
            }
            var appSwitches = GetSwitches(args);
            bool UseQuietMode = (appSwitches & (int)AppSwitches.QUIET_MODE) == (int)AppSwitches.QUIET_MODE;

            if ((appSwitches & (int)AppSwitches.SHOW_HELP) == (int)AppSwitches.SHOW_HELP)
            {
                ShowHelp(resourceManager);
                return;
            }
            if ((appSwitches & (int)AppSwitches.SHOW_ABOUT) == (int)AppSwitches.SHOW_ABOUT)
            {
                ShowAbout(resourceManager);
                return;
            }

            if (args.Length > 0)
            {
                var addressList = GetMacAddressList(args);
                if(addressList.Count > 0)
                {
                    foreach (var address in addressList)
                    {
                        if (!UseQuietMode)
                            Console.Out.WriteLine($"Sending WOL packet to [{address}]");
                        SendWOL(address);
                    }
                }
                else
                {
                    if (!UseQuietMode)
                        ShowHelp(resourceManager);
                }
            }
            else
            {
                ShowHelp(resourceManager);
            }
        }
    }
}
