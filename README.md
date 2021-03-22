<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
***
***
***
*** To avoid retyping too much info. Do a search and replace for the following:
*** RaeldZues, ServerClient, N/A, email, ServerClient, Simple implant client to get commands and execute them.
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/RaeldZues/ServerClient">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">ServerClient</h3>

  <p align="center">
    Simple implant client to get commands and execute them.
    <br />
    <a href="https://github.com/RaeldZues/ServerClient"><strong>Explore the docs �</strong></a>
    <br />
    <br />
    <a href="https://github.com/RaeldZues/ServerClient">View Demo</a>
    �
    <a href="https://github.com/RaeldZues/ServerClient/issues">Report Bug</a>
    �
    <a href="https://github.com/RaeldZues/ServerClient/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary><h2 style="display: inline-block">Table of Contents</h2></summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
Small inprogress project to start learning how to build an interactive shell on a remote windows machine. 

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites
* Windows machine you have full permission to execute programs with administrative privledges on
* Visual studio 2019 to build  
* nc to initiate connection to the client

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/RaeldZues/ServerClient.git
   ```
2. Build 

* Open and build ServerClient.sln with vs2019

<!-- USAGE EXAMPLES -->
# Usage
## Remote Windows Host you wish to run commands on:
* open administrative cmd prompt 
* traverse to executable 
* execute ```ServerClient.exe``` 

## Local Host you wish to send your commands from: 
* execute nc command to port 8080 (current default)
```sh 
nc <Remote Windows Machine IpAddress> <8080>
```
* Example cmd command ```cmd.exe /c whoami``` responds with username of Remote host 
* Example powershell command ```powershell.exe -command "[System.Security.Principal.WindowsIdentity]::GetCurrent().Name"``` responds with username of remote host.
* Unrecognized commands are echoed back to the nc instance 

<!-- ROADMAP -->
## Roadmap

* Fix return output from 
See the [open issues](https://github.com/RaeldZues/ServerClient/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Project Link: [https://github.com/RaeldZues/ServerClient](https://github.com/RaeldZues/ServerClient)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* []()
* []()
* []()





<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/RaeldZues/ServerClient.svg?style=for-the-badge
[contributors-url]: https://github.com/RaeldZues/ServerClient/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/RaeldZues/ServerClient.svg?style=for-the-badge
[forks-url]: https://github.com/RaeldZues/ServerClient/network/members
[stars-shield]: https://img.shields.io/github/stars/RaeldZues/ServerClient.svg?style=for-the-badge
[stars-url]: https://github.com/RaeldZues/repo/stargazers
[issues-shield]: https://img.shields.io/github/issues/RaeldZues/ServerClient.svg?style=for-the-badge
[issues-url]: https://github.com/RaeldZues/ServerClient/issues
[license-shield]: https://img.shields.io/github/license/RaeldZues/ServerClient.svg?style=for-the-badge
[license-url]: https://github.com/RaeldZues/ServerClient/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/NOLINKEDINCONNECTION