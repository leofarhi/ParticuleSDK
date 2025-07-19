from abc import ABC, abstractmethod

class Installer(ABC):
    """
    Base abstract installer.
    All installers must implement these methods.
    """

    @abstractmethod
    def install(self) -> None:
        """Install the distribution."""
        pass

    @abstractmethod
    def is_installed(self) -> bool:
        """Check if the distribution is installed."""
        pass

    @abstractmethod
    def configure(self) -> None:
        """Configure the distribution."""
        pass

    @abstractmethod
    def update(self) -> None:
        """Update the distribution."""
        pass

    @abstractmethod
    def uninstall(self) -> None:
        """Uninstall the distribution."""
        pass
